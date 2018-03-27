## Objective-C

### TaggedPointer：

Apple 在推进 64 位系统时，提出的一种内存优化指针管理方式。因为 64 位系统中，一个指针的大小是 8 个字节。在TaggedPointer 中，并不是传统意义上的指向对象的指针，而是拿出 4 个字节直接存储对象的数据，从而减少在堆上重新申请对象对内存产生的浪费，通常用在例如 NSNumber、NSData 等小对象中。

所以在以后，不要再使用 isa 来访问对象的类，而是使用 Cocoa 提供的 API 进行访问。

### 三种类型的Block：

Block 分为三种类型：全局Block（_NSConcreteGlobalBlock）、栈中Block（_NSConcreteStackBlock）堆中Block（_NSConcreteMallocBlock）

其中，这三种Block 的区别如下所示：
* 全局Block ：该Block 要么是空Block，要么是不访问任何外部变量的Block。它既不在栈中，也不在堆中，我理解为它可能在内存的全局区。
* 栈中Block ：该Block 有访问外部变量，并且该Block 只有一次执行，因为栈中的空间是可重复使用的，所以当栈中的Block 执行一次之后就被清除出栈了，所以无法多次使用。
* 堆中Block ：保存在堆中的Block，当引用计数为0时被销毁。该类型的Block 都是由栈中Block 从栈中复制到堆中形成的。ARC 模式下一般情况下不需要手动调用 copy。

### KVC 和 KVO

在使用 KVC 和 KVO 的时候，对于 key/keyPath 来说，均可以传入不带下划线的属性值或者带下划线的成员变量值。例如，在 Person 类中，有一个 name 属性：

```objc
@property (nonatomic, copy) NSString *name;
```

使用 KVC 的时候，以下两种方式均可：

```objc
[person setValue:@"Lin" forKey:@"name"];
[person setValue:@"Frank" forKey:@"_name"];
```

只不过第一种会调用 person 的 setter 方法，第二种不会调用。

对于未以属性方式生命的成员变量，KVC 也可以使用这两种方式进行赋值。例如往 Person 类中添加 _sex 成员变量：

```objc
@interface Person : NSObject {
    NSString *_sex;
}
...
@end
```

以下两种方式均能赋值：

```objc
[person setValue:@"Boy" forKey:@"sex"];
[person setValue:@"Man" forKey:@"_sex"];
```

若手动为 _sex 成员变量添加 setter 方法，则第一种方式也会访问其 setter 方法。

由此可知 KVC 的访问机制为：

首先根据 key 查看是否有相应的 setter 方法，若没有相应的 setter 方法，则判断类方法 accessInstanceVariablesDirectly 的返回值来根据 _\<key\>、_is\<key\>、\<key\>、is\<key\> 的顺序对成员变量进行访问，若都访问不到，则调用 setValue:forUndefinedKey:

在 KVO 访问某个属性/成员变量的时候，会首先生成对应的 setter 方法，然后在生成的方法中调用 willChange... 和 didChange... 方法来通知监听者。所以判断 KVO 能不能获取到值的监听，就是看在改变值的时候有没有调用相应的 setter 方法。

### 单例模式：

OC 中的单例模式实现起来比较简单，通常，最简单的做法是声明一个 `sharedInstance` 方法，然后在该方法的实现中进行单例对象的创建：

```objc

+ (void)sharedInstance {
    static <Class> *instance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        instance = [[<Class> alloc] init];
    });
    return instance;
}

```

但是这样会有一个问题，外界仍能通过调用 `alloc` 方法来创建新的对象。

所以有一种比较合适的做法，是在 `allocWithZone` 方法中进行单例静态变量的创建，然后在 `init` 方法中，使用 `dispatch_once` 对对象进行初始化，然后在 `sharedInstance` 方法中直接返回 `[[<Class> alloc] init]` 即可。

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
* enumerateObjectsUsingBlock / withOptions ...（需要取用index 时使用，或者小数据量时使用，自动添加 autoreleasepool）
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

### NSTimer 的持有释放问题

在开发中，`NSTimer` 是比较常用的一个控件，但是 `NSTimer` 通常会出现内存泄漏的问题。只是由于 `NSTimer` 添加到 `RunLoop` 中的时候，`RunLoop` 会对 `NSTimer` 有一个强引用，这样 `NSTimer` 就不会被释放，从而会出现内存泄漏的问题。当调用 `NSTimer` 的 `invalidate` 方法的时候，`RunLoop` 就不会继续持有该 `NSTimer` 对象，从而可以释放内存。
   
## 底层

### 静态库与动态库的区别：

静态库是指在链接时，会被完整地复制到可执行文件中的库，被多次使用就会多次冗余拷贝；（一堆目标文件的打包体，并非二进制文件）

动态库是指链接的时候不进行复制，程序运行的时候由系统动态地加载到内存中去，只加载一次，可以多个程序共用，节省内存。（但是由于iOS 系统沙盒机制以及需要对 IPA 进行签名，所以应用之间无法共享非官方动态库，只能在App Extension 与主App 之间共享动态库——Embedded Framework）

iOS 中，可以使用静态库和动态库。其中：

* 静态库以 `.a` 或者 `.framework` 为结尾（实际上就是一个压缩包）
* 动态库以 `.dylib` 或者 `.framework` 为结尾

具体操作在链接时进行，链接时决定每个目标文件的符号地址，若符号来自静态库，则将其纳入链接产物，并确定符号地址；若符号来自动态库，则打个标记，在启动时再进行加载。

使用 `CocoaPods` 的时候，生成的是动态库，但是并不能在工程中看到 `Embedded Framework` 中有嵌入动态库，是因为 `CocoaPods` 默认执行脚本，将这些动态库复制近 `.app` 中的 `FrameWork` 中。

## UIView

### UIButton 相关小 tip

UIButton 有很多中类型（实际上会创建UIButton 的子类，工厂模式），其中UIButtonTypeCustom 与 UIButtonTypeSystem 比较类似，但是 UIButtonTypeCustom 默认无点击高亮效果，UIButtonTypeSystem 有点击高亮效果。当 UIButton 实例有 UIEventTouchUpInside 类型的 target 和 action 的时候，添加一个点击手势（UITapGestureRecognizer）会使其添加的 target 无效。（断定为手势冲突）

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

### 动画的 fillMode 属性：

fillMode 属性决定了当前对象在非 active 时间段的行为，比如动画开始前和动画开始之后。

* kCAFillModeRemoved 默认值，动画对 `layer` 没有影响
* kCAFillModeForwards 动画向前延伸（即 `layer` 会一直保持动画最终的状态）
* kCAFillModeBackwards 在动画开始前，`layer` 会保持着动画一开始的样式

## AVFoundation

### AVCaptureSession 的朝向问题：

在AVCaptureSession 中，由于摄像头硬件原因，导致使用AVCapturePhotoOutput 捕获图片时会出现图片朝向不正确的问题，通过AVCaptureConnection 修改朝向之后，却没有效果（我认为可能原因是该Output 不支持修改朝向），最终采用通过修改已捕获的图片朝向来完成修改：
```objc
    UIImage *image = [UIImage imageWithCGImage:photo.CGImageRepresentation scale:1.0 orientation:UIImageOrientationRight];

```
