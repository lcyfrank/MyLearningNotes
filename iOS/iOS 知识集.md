## AutoLayout

### translatesAutoresizingMaskIntoConstraints 属性：

当superView 是使用自动布局，而subview 使用frame 布局的时候，AutoLayout 会将frame 的数据转换成自动布局

当这个属性设置为NO 的时候，Auto Layout 就不会将frame 转换，而是忽略

### SafeArea:

在每一个View 中，都可以有一个SafeArea，用来对子视图进行布局控制。（通过safeAreaLayoutGuide 属性来指定）

当视图未显示的时候，视图的safe area 的大小同视图的大小相同。

父视图对子视图的safe area 的边距（insets）可能有如下影响：

* 如果子视图完全在父视图的safe area 内部，则子视图的safe area 边界不受影响
* 如果子视图超出了父视图控制器的safe area，那么子视图的safe area 的取值会受父视图影响，其值为min(超出的边距，父视图的insets)

SafeArea 示例：

```objc
    self.vvv.translatesAutoresizingMaskIntoConstraints = NO;
    [self.vvv.topAnchor constraintEqualToSystemSpacingBelowAnchor:self.view.safeAreaLayoutGuide.topAnchor multiplier:1].active = YES;
    [self.vvv.bottomAnchor constraintEqualToSystemSpacingBelowAnchor:self.view.safeAreaLayoutGuide.bottomAnchor multiplier:1].active = YES;
    [self.vvv.leadingAnchor constraintEqualToSystemSpacingAfterAnchor:self.view.safeAreaLayoutGuide.leadingAnchor multiplier:1].active = YES;
    [self.vvv.trailingAnchor constraintEqualToSystemSpacingAfterAnchor:self.view.safeAreaLayoutGuide.trailingAnchor multiplier:1].active = YES;

```

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

## AVFoundation

### AVCaptureSession 的朝向问题：

在AVCaptureSession 中，由于摄像头硬件原因，导致使用AVCapturePhotoOutput 捕获图片时会出现图片朝向不正确的问题，通过AVCaptureConnection 修改朝向之后，却没有效果（我认为可能原因是该Output 不支持修改朝向），最终采用通过修改已捕获的图片朝向来完成修改：
```objc
    UIImage *image = [UIImage imageWithCGImage:photo.CGImageRepresentation scale:1.0 orientation:UIImageOrientationRight];

```
