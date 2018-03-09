## Objective-C

### 三种类型的Block：

Block 分为三种类型：全局Block（_NSConcreteGlobalBlock）、栈中Block（_NSConcreteStackBlock）堆中Block（_NSConcreteMallocBlock）

其中，这三种Block 的区别如下所示：
* 全局Block ：该Block 要么是空Block，要么是不访问任何外部变量的Block。它既不在栈中，也不在堆中，我理解为它可能在内存的全局区。
* 栈中Block ：该Block 有访问外部变量，并且该Block 只有一次执行，因为栈中的空间是可重复使用的，所以当栈中的Block 执行一次之后就被清除出栈了，所以无法多次使用。
* 堆中Block ：保存在堆中的Block，当引用计数为0时被销毁。该类型的Block 都是由栈中Block 从栈中复制到堆中形成的。

### NSBlockOperation：

NSBlockOperation 可以添加多个操作。如果添加的操作数量大于一，那么这些操作就会在 NSBlockOperation 对象中异步执行，而不考虑队列的同时并发数。（即只是在这个操作中异步，而整个操作在队列中仍遵循规则）

### +initialize 方法和 +load 方法：

* +initialize 方法在类或子类收到第一条消息之前被调用，如果程序一直没有给某个类**或它的子类**发送消息，那么这个类的 +initialize 方法就永远不会被调用；采用`objc_msgSend`机制进行调用，也就是说如果子类没有实现该方法，会继承调用父类的方法，且当分类中实现了该方法时，会覆盖主类方法的调用

* +load 方法是当类或者分类被添加到Objective-C 的runtime 中的时候被调用，子类的该方法会在父类该方法执行完之后执行，分类该方法会在主类该方法执行完之后执行；采用函数指针对该方法进行直接调用，也就是说子类、父类、分类中的该方法被区别对待。

### Object、Class、MetaClass：

在Objective-C 中，一切对象都是类（当然除了int、double 等）在Objective-C 的对象结构体数据中，每一个对象都有一个isa 指针，指向他的类型，在调用对象的方法时，运行时就通过isa 指针，寻找其指向的类，从而获取方法表中的方法进行调用。而对于Class 来说，因为Class 也是一个对象，所以每个Class 也有一个isa 指针，指向成为MetaClass （元类）的类，这样在调用类方法时，运行时会通过Class 中的isa 指针找到MetaClass。而对于每一个MetaClass，他们的isa 指针均指向一个 **Root Meta Class**，Root Meta Class 的isa 指针指向其自己。三者示意图如下：

![](../res/class-diagram.jpg)

演示代码如下：
```objc
    Class cls = object_getClass(baseView);  // get isa
    Class meta_cls = object_getClass(cls);  // get isa
    Class root_meta_cls = object_getClass(meta_cls);  // get isa

    NSLog(@"类 [%@ : %p] --- 父类 [%@ : %p]", cls, cls,
          [cls superclass], [cls superclass]);  // class
    
    NSLog(@"元类 [%@ : %p] --- 父类的元类 [%@ : %p]", meta_cls, meta_cls,
          [meta_cls superclass], [meta_cls superclass]);  // meta class
    
    NSLog(@"根元类 [%@ : %p] --- 根元类的父类 [%@ : %p]", root_meta_cls, root_meta_cls,
          [root_meta_cls superclass], [root_meta_cls superclass]);  // root meta class
```

打印结果如下所示：

    类 [UIScrollView : 0x112367f88] --- 父类 [UIView : 0x112367ab0]
    元类 [UIScrollView : 0x112367fb0] --- 父类的元类 [UIView : 0x112367c18]
    根元类 [NSObject : 0x10e87ee58] --- 根元类的父类 [NSObject : 0x10e87eea8]

### 集合遍历方法：

在Objective-C 中，有以下几种集合遍历的方法：
* 传统C 语言的for 循环遍历
* for-in 格式语法，即NSFastEnumeration（大数据量时很快，因为有一定并行性）
* makeObjectsPerformSelector... 方法遍历（需要执行相应方法）
* 集合运算符（速度很慢，写法简洁）
* enumerateObjectsUsingBlock / withOptions ...（需要取用index 时使用，或者小数据量时使用）
* dispatch_apply（支持并行，在无序且每个对象耗时操作时使用）

参考链接：http://blog.sunnyxx.com/2014/04/30/ios_iterator/

### 可变参数：

使用 va_list、va_start、va_arg、va_end 四个宏来对可变参数的方法以及函数的参数进行提取操作。**但要注意的是在向可变参数方法/函数传入参数的时候，最后一个参数需为nil，来表示可变参数结束。**

原理：在函数/方法的参数入栈中，会按照参数从右到左的顺序将参数依次连续入栈，所以在选取参数的时候根据第一个参数的地址，可以找到后续参数的地址。但是由于计算机的内存对齐机制，直接采用偏移地址的方式取得参数会出现错误，故用以上四个宏来操作。（栈的地址是从高到低延伸，栈底地址最大）

### 集合的拷贝问题：

在 Objective-C 中，对于集合的拷贝操作，需要调用 copy / mutableCopy 方法进行。但是在实践中发现，拷贝操作中，对于不可变集合（NSArray、NSDictionary、NSString）调用 copy 方法时，返回的仍然是原实例本身，只有对不可变集合调用 mutableCopy 或者对可变集合（NSMutableArray、NSMutableString）调用 copy / mutableCopy 方法时，集合才会执行拷贝操作。

（深拷贝与浅拷贝）

在集合的拷贝过程中，对于集合中的元素，默认采用的是浅拷贝，即集合中的元素仍为原来的元素，不会被进行拷贝操作。即使调用[NSArray arrayWithArray:...] 方法，得到的新数组中的元素仍指向原数组。若要实现数组中元素的拷贝操作，则需要调用 initWithArray:copyItems: 方法进行，这样数组在新建过程中，会调用数组中元素的 copy 方法进行拷贝

```objc
    NSString *str = @"fff";
    NSMutableString *mutable_str = [@"fff" mutableCopy];
    
    NSMutableArray *a1 = [@[str, mutable_str] mutableCopy];
    NSArray *a2 = [a1 copy];
    NSArray *a3 = [[NSArray alloc] initWithArray:a1 copyItems:YES];
    
    NSLog(@"a1: %p, a2: %p", a1, a2);  // array mutable copy
    NSLog(@"original: %p, %p", a1[0], a1[1]);
    NSLog(@"copied: %p, %p", a3[0], a3[1]);  // deep copy of elements
```

    a1: 0x608000057c10, a2: 0x60800002afa0
    original: 0x10f8d3398, 0x608000057c40
    copied: 0x10f8d3398, 0xa000000006666663

## UIView

### bounds 与frame 的关系：

frame 表示的是视图在其父视图的坐标轴的坐标

bounds 表示的是视图在自身坐标轴的坐标

当改变bounds 的x、y 值的时候，并不会改变自身在父视图中的位置（即frame），但是会影响子视图在自身的位置

当改变bounds 的大小的时候，会影响frame 值（以自身中点为基准缩放）

**frame 与bounds 与position 与anchorPoint 的公式如下：**

* frame.origin.x = position.x - anchorPoint.x * bounds.size.width

* frame.origin.y = position.y - anchorPoint.y * bounds.size.height

### UIMenuController 显示：

菜单UIMenuController 的显示需要满足一些条件。在默认情况下，**UITextField** 等控件默认支持菜单显示。其他不支持显示菜单项的控件，可以通过重写来使其支持菜单项的显示。在控件中，重写 canBecomeFirstResponder 返回为YES，使控件可以成为FirstResponder，这样控件就能支持菜单的显示。在需要显示菜单项的时候，首先调用需要显示菜单的控件的 becomeFirstResponder 方法，使其成为第一响应者，然后调用菜单的 setTarget... 方法，指定显示的坐标以及控件，最后调用菜单 setVisible... 方法，将菜单显示出来。
在菜单项的控件中，还有一个方法 canPerformAction... 也与菜单项相关。在该方法中，可以对指定的操作进行判断并指明显示还是不显示。在实验中发现，如果显示菜单的控件不实现该方法，默认会调用控件所在控制器的该方法来进行筛选。

```objc
    // 显示菜单项
    [self.clsView becomeFirstResponder];
    [[UIMenuController sharedMenuController] setTargetRect:CGRectMake(50, 50, 200, 30) inView:self.clsView];
    [[UIMenuController sharedMenuController] setMenuVisible:YES animated:YES];
```


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

## AVFoundation

### AVCaptureSession 的朝向问题：

在AVCaptureSession 中，由于摄像头硬件原因，导致使用AVCapturePhotoOutput 捕获图片时会出现图片朝向不正确的问题，通过AVCaptureConnection 修改朝向之后，却没有效果（我认为可能原因是该Output 不支持修改朝向），最终采用通过修改已捕获的图片朝向来完成修改：
```objc
    UIImage *image = [UIImage imageWithCGImage:photo.CGImageRepresentation scale:1.0 orientation:UIImageOrientationRight];

```
