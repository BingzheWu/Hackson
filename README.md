# Hackson
灵感来源：
拍照是我们游玩过程中乐趣的一大来源。有时在游览路上匆匆拍下的风景或人，回来整理照片时却发现“惨不忍睹”——地平线歪了，人物身体部位被截，或者构图过于奇怪……删去觉得可惜，不删照片又实在不够美观。如果在拍照时就能意识到这个问题并作出调整，大概就能少掉很多遗憾。这个app正是基于这种生活场景而被创造出来的。

Inspirations：
Taking photos is one of the greatest sources of pleasure during travelling. When we hastily grab our phones and press the button of camera, however, the effect is not always satisfactory. Stilted horizon, incomplete figures or strange composition of pictures lead the photo to the edge of being deleted. If we can realize the problem at the filming moment and make adjustment immediately, much regret may never occur. This app is developed based on this common situation and help people with little knowledge of photography to make fair photos.
  
特点：
应用智能机器视觉算法，检测照片中的标识物体（比如地平线，人脸，建筑等）的位置，对构图做出决策，指导拍照者的拍照
Features：
We used the Intelligent machine vision algorithm to detect the location of specific items in the photo (eg. Horizon, face, architecture etc). By doing this, we can propose suggestions for the photo taker about the composition of pictures.
 
挑战：实现地平线检测的时候，调研了这个领域，没有发现有效且鲁棒性（Robust）较好的算法，最后选择了一种基于数据的机器学习算法，但是会占用较多的计算资源，但最后通过调整参数，还是实现了实时的检测。在本次比赛中，我们在人脸检测上也遇到了不小的麻烦，最开始打算使用微软提供的 API接口，但是使用后发现网络传输速度大大影响了软件的使用体验，最后我们决定使用基于Haar特征的算法。
Challenges：
To realize the detection of the horizon, we surveyed the area without finding effective and robust algorithm. Finally we chose a machine-learning algorithm based on data, which, however, would take up much calculus resources. Finally we make the real-time detection possible by adjusting the parameters. In addition, we also met great difficulty in human face detection. Initially we intended to use the API offered by Microsoft, but it proved not to be optimal since the transmission speed of the web greatly weakened the user experience. At last we decided to use the algorithm based on Haar features.

未来发展方向:
可以从算法和硬件两个方面发展，完善产品的功能。算法方面可以利用深度学习的算法实现对拍照场景，物体等识别，然后根据拍照者所在环境，智能推荐构图方法。硬件方面，可以通过双目摄像头实现对景深的探测，从而实现更美观的智能构图推荐。
Futures
The function of our product can be improved both through algorithm and hardware. In algorithm, that of deep learning can be used to realize the recognition of location and items and based on the surroundings of the photographer to recommend the optimal composition. In hardware, Binocular cameras will be of use in detection of the depth of field so as to produce better composition recommend. 

用到的技术：
我们基于Linux系统和相应的WebCamera实现了我们的demo。主要的编程语言为C++，在实现算法的过程中主要使用了OpenCV的一些主要API接口。
软件方面：本次主要用到两个主要的算法，一个是基于AdaBoost实现人脸识别的Haar特征检测算法，一个是基于SVM的地平线检测算法。其他还使用了边缘检测的canny算子。
Built with：
C++, OpenCV, Machine Learning, Edge Detection


Ref：
【1】	Ahmad T, Bebis G, Regentova E E, et al. A Machine Learning Approach to Horizon Line Detection Using Local Features[C]// International Symposium on Visual Computing. 2013:181-193.
【2】	An Extended Set of Haar-like Features for Rapid Object Detection
