# ComplexionDetector

**注意：**

- fork这个仓库，在自己的仓库调试，提交的话在这个仓库请求合并
- 你也可以新建一个dev分支，push到dev分支上，push前请pull，如果有冲突，解决了后再重新push
- 用x64编译

[git教程](https://www.runoob.com/git/git-tutorial.html)

**git命令举例**

```shell
对某个分支进行push和pull等操作时，记得先切换到该分支

// 第一次先clone仓库
git clone git@github.com:hominsu/ComplexionDetector.git

// 列出当前所有分支
git branch

// 查看每一个分支的最后一次提交
git branch -v

// 切换到dev分支
git checkout dev

// 检查dev分支有无更新
git pull origin dev

// 查看在上次提交之后是否有对文件进行再次修改
git status

// 添加一个或多个文件到暂存区
git add 文件1 文件2

// 添加当前目录下的所有文件到暂存区：
git add .

// 将暂存区内容添加到仓库中
git commit -m "更新内容"

// push到远端的dev分支
git push origin dev

```

**Github团队协作开发**

- [怎么团队合作多人开发项目](https://blog.csdn.net/dietime1943/article/details/81391835)
- [SSH配置](https://zhuanlan.zhihu.com/p/67814301)
- 你可以通过Fork & Pull request方式来申请合并
- 你也可以直接push到dev分支

**vs+opencv+qt环境配置请参考以下文章**

- [基于VS2019配置opencv4.0](https://blog.csdn.net/qq_26884501/article/details/90770131)
- [Vs2019+Qt](https://blog.csdn.net/gaocui883/article/details/103965341)
