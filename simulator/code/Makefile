CC = g++
LD = g++
CC_FLAGS = -std=c++11 -O2 -I. -I./include/ -I/usr/include/freetype2/
LD_FLAGS = 
LIBS = -lglut -lGLEW -lglfw3 -lGLU -lGL -lX11 -ldl -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -lfreetype -lAntTweakBar

.PHONY: all
all: cs467

UTILS = out/Utils/BMPIO.o out/Utils/Image.o out/Utils/Logger.o out/Utils/Timer.o out/Utils/Utils.o

out/Utils/BMPIO.o: Utils/BMPIO.cpp
	$(CC) $(CC_FLAGS) -c -o out/Utils/BMPIO.o Utils/BMPIO.cpp

out/Utils/Image.o: Utils/Image.cpp
	$(CC) $(CC_FLAGS) -c -o out/Utils/Image.o Utils/Image.cpp

out/Utils/Logger.o: Utils/Logger.cpp
	$(CC) $(CC_FLAGS) -c -o out/Utils/Logger.o Utils/Logger.cpp

out/Utils/Timer.o: Utils/Timer.cpp
	$(CC) $(CC_FLAGS) -c -o out/Utils/Timer.o Utils/Timer.cpp

out/Utils/Utils.o: Utils/Utils.cpp
	$(CC) $(CC_FLAGS) -c -o out/Utils/Utils.o Utils/Utils.cpp

MATHLIB = out/MathLib/P3D.o out/MathLib/Plane.o out/MathLib/Quaternion.o out/MathLib/Ray.o out/MathLib/Segment.o out/MathLib/V3D.o

out/MathLib/P3D.o: MathLib/P3D.cpp
	$(CC) $(CC_FLAGS) -c -o out/MathLib/P3D.o MathLib/P3D.cpp

out/MathLib/Plane.o: MathLib/Plane.cpp
	$(CC) $(CC_FLAGS) -c -o out/MathLib/Plane.o MathLib/Plane.cpp

out/MathLib/Quaternion.o: MathLib/Quaternion.cpp
	$(CC) $(CC_FLAGS) -c -o out/MathLib/Quaternion.o MathLib/Quaternion.cpp

out/MathLib/Ray.o: MathLib/Ray.cpp
	$(CC) $(CC_FLAGS) -c -o out/MathLib/Ray.o MathLib/Ray.cpp

out/MathLib/Segment.o: MathLib/Segment.cpp
	$(CC) $(CC_FLAGS) -c -o out/MathLib/Segment.o MathLib/Segment.cpp

out/MathLib/V3D.o: MathLib/V3D.cpp
	$(CC) $(CC_FLAGS) -c -o out/MathLib/V3D.o MathLib/V3D.cpp

OPTIMIZATIONLIB = out/OptimizationLib/BFGSFunctionMinimizer.o out/OptimizationLib/BFGSHessianApproximator.o out/OptimizationLib/GradientBasedFunctionMinimizer.o out/OptimizationLib/GradientDescentFunctionMinimizer.o out/OptimizationLib/GreedyRandomizedMinimizer.o out/OptimizationLib/NewtonFunctionMinimizer.o out/OptimizationLib/ObjectiveFunction.o out/OptimizationLib/SoftUnilateralConstraint.o

out/OptimizationLib/BFGSFunctionMinimizer.o: OptimizationLib/BFGSFunctionMinimizer.cpp
	$(CC) $(CC_FLAGS) -c -o out/OptimizationLib/BFGSFunctionMinimizer.o OptimizationLib/BFGSFunctionMinimizer.cpp

out/OptimizationLib/BFGSHessianApproximator.o: OptimizationLib/BFGSHessianApproximator.cpp
	$(CC) $(CC_FLAGS) -c -o out/OptimizationLib/BFGSHessianApproximator.o OptimizationLib/BFGSHessianApproximator.cpp

out/OptimizationLib/GradientBasedFunctionMinimizer.o: OptimizationLib/GradientBasedFunctionMinimizer.cpp
	$(CC) $(CC_FLAGS) -c -o out/OptimizationLib/GradientBasedFunctionMinimizer.o OptimizationLib/GradientBasedFunctionMinimizer.cpp

out/OptimizationLib/GradientDescentFunctionMinimizer.o: OptimizationLib/GradientDescentFunctionMinimizer.cpp
	$(CC) $(CC_FLAGS) -c -o out/OptimizationLib/GradientDescentFunctionMinimizer.o OptimizationLib/GradientDescentFunctionMinimizer.cpp

out/OptimizationLib/GreedyRandomizedMinimizer.o: OptimizationLib/GreedyRandomizedMinimizer.cpp
	$(CC) $(CC_FLAGS) -c -o out/OptimizationLib/GreedyRandomizedMinimizer.o OptimizationLib/GreedyRandomizedMinimizer.cpp

out/OptimizationLib/NewtonFunctionMinimizer.o: OptimizationLib/NewtonFunctionMinimizer.cpp
	$(CC) $(CC_FLAGS) -c -o out/OptimizationLib/NewtonFunctionMinimizer.o OptimizationLib/NewtonFunctionMinimizer.cpp

out/OptimizationLib/ObjectiveFunction.o: OptimizationLib/ObjectiveFunction.cpp
	$(CC) $(CC_FLAGS) -c -o out/OptimizationLib/ObjectiveFunction.o OptimizationLib/ObjectiveFunction.cpp

out/OptimizationLib/SoftUnilateralConstraint.o: OptimizationLib/SoftUnilateralConstraint.cpp
	$(CC) $(CC_FLAGS) -c -o out/OptimizationLib/SoftUnilateralConstraint.o OptimizationLib/SoftUnilateralConstraint.cpp

GUILIB = out/GUILib/FreeType.o out/GUILib/GLApplication.o out/GUILib/GLCamera.o out/GUILib/GLConsole.o out/GUILib/GLContentManager.o out/GUILib/GLMesh.o out/GUILib/GLShader.o out/GUILib/GLShaderMaterial.o out/GUILib/GLTexture.o out/GUILib/GLTrackingCamera.o out/GUILib/GLUtils.o out/GUILib/GLWindow.o out/GUILib/GLWindow2D.o out/GUILib/GLWindow3D.o out/GUILib/GLWindowContainer.o out/GUILib/GlobalMouseState.o out/GUILib/InteractiveWidget.o out/GUILib/OBJReader.o out/GUILib/RotateWidget.o out/GUILib/RotateWidgetV1.o out/GUILib/RotateWidgetV2.o out/GUILib/TransformWidget.o out/GUILib/TranslateWidget.o

out/GUILib/FreeType.o: GUILib/FreeType.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/FreeType.o GUILib/FreeType.cpp

out/GUILib/GLApplication.o: GUILib/GLApplication.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/GLApplication.o GUILib/GLApplication.cpp

out/GUILib/GLCamera.o: GUILib/GLCamera.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/GLCamera.o GUILib/GLCamera.cpp

out/GUILib/GLConsole.o: GUILib/GLConsole.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/GLConsole.o GUILib/GLConsole.cpp

out/GUILib/GLContentManager.o: GUILib/GLContentManager.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/GLContentManager.o GUILib/GLContentManager.cpp

out/GUILib/GLMesh.o: GUILib/GLMesh.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/GLMesh.o GUILib/GLMesh.cpp

out/GUILib/GLShader.o: GUILib/GLShader.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/GLShader.o GUILib/GLShader.cpp

out/GUILib/GLShaderMaterial.o: GUILib/GLShaderMaterial.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/GLShaderMaterial.o GUILib/GLShaderMaterial.cpp

out/GUILib/GLTexture.o: GUILib/GLTexture.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/GLTexture.o GUILib/GLTexture.cpp

out/GUILib/GLTrackingCamera.o: GUILib/GLTrackingCamera.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/GLTrackingCamera.o GUILib/GLTrackingCamera.cpp

out/GUILib/GLUtils.o: GUILib/GLUtils.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/GLUtils.o GUILib/GLUtils.cpp

out/GUILib/GLWindow.o: GUILib/GLWindow.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/GLWindow.o GUILib/GLWindow.cpp

out/GUILib/GLWindow2D.o: GUILib/GLWindow2D.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/GLWindow2D.o GUILib/GLWindow2D.cpp

out/GUILib/GLWindow3D.o: GUILib/GLWindow3D.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/GLWindow3D.o GUILib/GLWindow3D.cpp

out/GUILib/GLWindowContainer.o: GUILib/GLWindowContainer.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/GLWindowContainer.o GUILib/GLWindowContainer.cpp

out/GUILib/GlobalMouseState.o: GUILib/GlobalMouseState.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/GlobalMouseState.o GUILib/GlobalMouseState.cpp

out/GUILib/InteractiveWidget.o: GUILib/InteractiveWidget.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/InteractiveWidget.o GUILib/InteractiveWidget.cpp

out/GUILib/OBJReader.o: GUILib/OBJReader.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/OBJReader.o GUILib/OBJReader.cpp

out/GUILib/RotateWidget.o: GUILib/RotateWidget.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/RotateWidget.o GUILib/RotateWidget.cpp

out/GUILib/RotateWidgetV1.o: GUILib/RotateWidgetV1.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/RotateWidgetV1.o GUILib/RotateWidgetV1.cpp

out/GUILib/RotateWidgetV2.o: GUILib/RotateWidgetV2.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/RotateWidgetV2.o GUILib/RotateWidgetV2.cpp

out/GUILib/TransformWidget.o: GUILib/TransformWidget.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/TransformWidget.o GUILib/TransformWidget.cpp

out/GUILib/TranslateWidget.o: GUILib/TranslateWidget.cpp
	$(CC) $(CC_FLAGS) -c -o out/GUILib/TranslateWidget.o GUILib/TranslateWidget.cpp

ASSIGNMENT2 = out/Assignment2/CollisionPlane.o out/Assignment2/PBFApp.o out/Assignment2/ParticleSystem.o out/Assignment2/ParticleSystemLoader.o out/Assignment2/SpatialMap.o out/Assignment2/KDTree.o out/Assignment2/main.o

out/Assignment2/CollisionPlane.o: Assignment2/CollisionPlane.cpp
	$(CC) $(CC_FLAGS) -c -o out/Assignment2/CollisionPlane.o Assignment2/CollisionPlane.cpp

out/Assignment2/PBFApp.o: Assignment2/PBFApp.cpp
	$(CC) $(CC_FLAGS) -c -o out/Assignment2/PBFApp.o Assignment2/PBFApp.cpp

out/Assignment2/ParticleSystem.o: Assignment2/ParticleSystem.cpp
	$(CC) $(CC_FLAGS) -c -o out/Assignment2/ParticleSystem.o Assignment2/ParticleSystem.cpp

out/Assignment2/ParticleSystemLoader.o: Assignment2/ParticleSystemLoader.cpp
	$(CC) $(CC_FLAGS) -c -o out/Assignment2/ParticleSystemLoader.o Assignment2/ParticleSystemLoader.cpp

out/Assignment2/SpatialMap.o: Assignment2/SpatialMap.cpp
	$(CC) $(CC_FLAGS) -c -o out/Assignment2/SpatialMap.o Assignment2/SpatialMap.cpp

out/Assignment2/KDTree.o: Assignment2/KDTree.cpp
	$(CC) $(CC_FLAGS) -c -o out/Assignment2/KDTree.o Assignment2/KDTree.cpp

out/Assignment2/main.o: Assignment2/main.cpp
	$(CC) $(CC_FLAGS) -c -o out/Assignment2/main.o Assignment2/main.cpp

OBJS = $(UTILS) $(MATHLIB) $(OPTIMIZATIONLIB) $(GUILIB) $(ASSIGNMENT2)

Utils_dir:
	mkdir -p out/Utils

MathLib_dir:
	mkdir -p out/MathLib

OptimizationLib_dir:
	mkdir -p out/OptimizationLib

GUILib_dir:
	mkdir -p out/GUILib

Assignment2_dir:
	mkdir -p out/Assignment2

.PHONY: directories
directories:  Utils_dir MathLib_dir OptimizationLib_dir GUILib_dir Assignment2_dir

cs467: directories $(OBJS)
	$(LD) $(LD_FLAGS) $(OBJS) -L../libs -lglut -L../libs -lGLEW -L../libs -lglfw3 -lGLU -lGL -lX11 -ldl -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -pthread -lfreetype -L/home/cc/cs184/sp19/class/cs184-aci/AntTweakBar/lib -lAntTweakBar -o out/cs467

clean:
	rm -r out/*
