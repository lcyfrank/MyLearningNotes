## AutoLayout

### translatesAutoresizingMaskIntoConstraints 属性：

当superView 是使用自动布局，而subview 使用frame 布局的时候，AutoLayout 会将frame 的数据转换成自动布局

当这个属性设置为NO 的时候，Auto Layout 就不会将frame 转换，而是忽略

## CoreAnimation

### masksToBounds 属性：

cornerRadius 只对**背景色**和**图层边框**有用，对layer 的contents 属性无效，当内容背景不是透明的时候，cornerRadius 可能会没有效果，所以需要设置masksToBounds 为YES 来解决，masksToBounds 作用是决定子layer 是否被当前layer 的边界裁剪


## UIView

### bounds 与frame 的关系：

frame 表示的是视图在其父视图的坐标轴的坐标

bounds 表示的是视图在自身坐标轴的坐标

当改变bounds 的x、y 值的时候，并不会改变自身在父视图中的位置（即frame），但是会影响子视图在自身的位置

当改变bounds 的大小的时候，会影响frame 值（以自身中点为基准缩放）

**frame 与bounds 与position 与anchorPoint 的公式如下：**

* frame.origin.x = position.x - anchorPoint.x * bounds.size.width

* frame.origin.y = position.y - anchorPoint.y * bounds.size.height
