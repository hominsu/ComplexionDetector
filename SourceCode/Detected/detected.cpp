#include "detected.h"



bool Detected::getisGpu()
{
    return isGpu;
}

double Detected::getusagetime()
{
    return usagetime;
}

int Detected::getClassId()
{
    return classId;
}

std::vector<torch::Tensor> Detected::non_max_suppression(torch::Tensor preds, float score_thresh = 0.01, float iou_thresh = 0.35, bool isGpu = true)
{
    std::vector<torch::Tensor> output;
    for (size_t i = 0; i < preds.sizes()[0]; ++i)
    {
        torch::Tensor pred = preds.select(0, i);

        //GPU推理结果为cuda数据类型，nms之前要转成cpu，否则会报错
        if (isGpu)
        {
            pred = pred.to(at::kCPU); //增加到函数里pred = pred.to(at::kCPU); 注意preds的数据类型，转成cpu进行后处理。
        }

        // Filter by scores
        torch::Tensor scores = pred.select(1, 4) * std::get<0>(torch::max(pred.slice(1, 5, pred.sizes()[1]), 1));
        pred = torch::index_select(pred, 0, torch::nonzero(scores > score_thresh).select(1, 0));
        if (pred.sizes()[0] == 0) continue;

        // (center_x, center_y, w, h) to (left, top, right, bottom)
        pred.select(1, 0) = pred.select(1, 0) - pred.select(1, 2) / 2;
        pred.select(1, 1) = pred.select(1, 1) - pred.select(1, 3) / 2;
        pred.select(1, 2) = pred.select(1, 0) + pred.select(1, 2);
        pred.select(1, 3) = pred.select(1, 1) + pred.select(1, 3);

        // Computing scores and classes
        std::tuple<torch::Tensor, torch::Tensor> max_tuple = torch::max(pred.slice(1, 5, pred.sizes()[1]), 1);
        pred.select(1, 4) = pred.select(1, 4) * std::get<0>(max_tuple);
        pred.select(1, 5) = std::get<1>(max_tuple);

        torch::Tensor  dets = pred.slice(1, 0, 6);

        torch::Tensor keep = torch::empty({ dets.sizes()[0] });
        torch::Tensor areas = (dets.select(1, 3) - dets.select(1, 1)) * (dets.select(1, 2) - dets.select(1, 0));
        std::tuple<torch::Tensor, torch::Tensor> indexes_tuple = torch::sort(dets.select(1, 4), 0, 1);
        torch::Tensor v = std::get<0>(indexes_tuple);
        torch::Tensor indexes = std::get<1>(indexes_tuple);
        int count = 0;
        while (indexes.sizes()[0] > 0)
        {
            keep[count] = (indexes[0].item().toInt());
            count += 1;

            // Computing overlaps
            torch::Tensor lefts = torch::empty(indexes.sizes()[0] - 1);
            torch::Tensor tops = torch::empty(indexes.sizes()[0] - 1);
            torch::Tensor rights = torch::empty(indexes.sizes()[0] - 1);
            torch::Tensor bottoms = torch::empty(indexes.sizes()[0] - 1);
            torch::Tensor widths = torch::empty(indexes.sizes()[0] - 1);
            torch::Tensor heights = torch::empty(indexes.sizes()[0] - 1);
            for (size_t i = 0; i < indexes.sizes()[0] - 1; ++i)
            {
                lefts[i] = std::max(dets[indexes[0]][0].item().toFloat(), dets[indexes[i + 1]][0].item().toFloat());
                tops[i] = std::max(dets[indexes[0]][1].item().toFloat(), dets[indexes[i + 1]][1].item().toFloat());
                rights[i] = std::min(dets[indexes[0]][2].item().toFloat(), dets[indexes[i + 1]][2].item().toFloat());
                bottoms[i] = std::min(dets[indexes[0]][3].item().toFloat(), dets[indexes[i + 1]][3].item().toFloat());
                widths[i] = std::max(float(0), rights[i].item().toFloat() - lefts[i].item().toFloat());
                heights[i] = std::max(float(0), bottoms[i].item().toFloat() - tops[i].item().toFloat());
            }
            torch::Tensor overlaps = widths * heights;

            // FIlter by IOUs
            torch::Tensor ious = overlaps / (areas.select(0, indexes[0].item().toInt()) + torch::index_select(areas, 0, indexes.slice(0, 1, indexes.sizes()[0])) - overlaps);
            indexes = torch::index_select(indexes, 0, torch::nonzero(ious <= iou_thresh).select(1, 0) + 1);
        }
        keep = keep.toType(torch::kInt64);
        output.push_back(torch::index_select(dets, 0, keep.slice(0, 0, count)));
    }
    return output;
}

void Detected::initalDetected()
{
    if (torch::cuda::is_available())
    {
        this->device_type = at::kCUDA;
        this->isGpu = true;
    }
    else
    {
        this->device_type = at::kCPU;
        this->isGpu = false;
    }
}

void Detected::loadingModule()
{
    if (isGpu)
    {
        module = torch::jit::load("model/last_gpu.torchscript.pt");
    }
    else
    {
        module = torch::jit::load("model/last.torchscript.pt");
    }

    module.to(this->device_type); // 模型加载至GPU或GPU

    std::ifstream f("model/action.names");
    std::string name = "";
    while (std::getline(f, name))
    {
        classnames.push_back(name);
    }
}

cv::Mat Detected::forward(const cv::Mat &src)
{
    cv::Mat frame, img;
    frame = src;
    int width = frame.size().width;
    int height = frame.size().height;
    
    auto start = std::chrono::system_clock::now();

    // Preparing input tensor
    cv::resize(frame, img, cv::Size(640, 480));
    cv::cvtColor(img, img, cv::COLOR_BGR2RGB);  // BGR -> RGB
    img.convertTo(img, CV_32FC3, 1.0f / 255.0f);  // normalization 1/255

    auto imgTensor = torch::from_blob(img.data, { 1, img.rows, img.cols, img.channels() }).to(device_type);
    imgTensor = imgTensor.permute({ 0, 3, 1, 2 }).contiguous();  // BHWC -> BCHW (Batch, Channel, Height, Width)

    std::vector<torch::jit::IValue> inputs;
    inputs.emplace_back(imgTensor);

    torch::jit::IValue output = module.forward(inputs);
    auto preds = output.toTuple()->elements()[0].toTensor();

    std::vector<torch::Tensor> dets = non_max_suppression(preds, 0.35, 0.5, isGpu);
    if (dets.size() > 0)
    {
        size_t maxScoreId = 0;
        float maxScore = 0;
        for (size_t i = 0; i < dets[0].sizes()[0]; ++i)
        {
            if (dets[0][i][4].item().toFloat() > maxScore)
            {
                maxScore = dets[0][i][4].item().toFloat();
                maxScoreId = i;
            }
        }
        float left = dets[0][maxScoreId][0].item().toFloat() * frame.cols / 640;
        float top = dets[0][maxScoreId][1].item().toFloat() * frame.rows / 480;
        float right = dets[0][maxScoreId][2].item().toFloat() * frame.cols / 640;
        float bottom = dets[0][maxScoreId][3].item().toFloat() * frame.rows / 480;
        float score = dets[0][maxScoreId][4].item().toFloat();
        int classID = dets[0][maxScoreId][5].item().toInt();

        std::cout << "left: " << left << " top: " << top << " right: " << right << " score: " << score << " ClassID: " << classID << std::endl;

        cv::rectangle(frame, cv::Rect(left, top, (right - left), (bottom - top)), cv::Scalar(0, 255, 0), 2);

        classId = classID;

        cv::putText(frame,
            classnames[classID] + ": " + cv::format("%.2f", score),
            cv::Point(left, top),
            cv::FONT_HERSHEY_SIMPLEX, (right - left) / 200, cv::Scalar(0, 255, 0), 2);
    }
    else
    {
        classId = -1;
    }
    auto diff = std::chrono::system_clock::now() - start;
    usagetime = diff.count() / 10000;

    //cv::imshow("", frame);
    //cv::resize(frame, frame, cv::Size(width, height));
    //if (cv::waitKey(1) == 27) break;
    return frame;
}

