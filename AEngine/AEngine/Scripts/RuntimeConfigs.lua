numGameobjects = 50;
scalerAnimTime = 0.50;
scalerIncrement= 0.20;
numWorkerThreads = 8;
animationSystem = WorldLib.GetAnimationSystem(world);
taskSystem = WorldLib.GetTaskSystem(world);

TaskSystemLib.SetNumWorkerThreads(taskSystem, numWorkerThreads);
WorldLib.RegisterGameObjects(world, numGameobjects);

-- function SetAnimScaler(gameObjectId, ScalerSpeed)
	-- if gameObjectId < numGameobjects then
		-- AnimationSystemLib.ChangeGameObjectAnimSpeed(animationSystem, i, scalerAnimTime);
	-- end
-- end

for i = 0, numGameobjects, 1 do
	AnimationSystemLib.ChangeGameObjectAnimSpeed(animationSystem, i, scalerAnimTime);
	--SetAnimScaler(i, scalerAnimTime);
	scalerAnimTime = scalerAnimTime + scalerIncrement;
end

