# UE Batch Profiler

> **Warning**
> This project is still work in progress

Either you want to optimize something or find the bottleneck, it is crucial to know the current
changes and use as much as the same conditions to better understand the output of the profiling.
In order to maintain this consistency, using the same camera setup on the same views are really
useful, but it is time consuming to do manually.

To better analyze the frame and have a more smooth capture process, I have created this plugin. It helps you to

- Have seperated _ProfilingCamera_ actors around the world you can use and adjust without messing with game cameras within the world
- Allows you to capture with _UE Insights_ and _RenderDoc_ (must be installed and enabled)
- Can capture active view or run a batch capture from all active profiling cameras
- Activate / Deactivate those cameras individually for batch capturing
- Can capture with custom resolutions and window modes
- Can execute console commands before and after capture such as `stat` commands
- Can delay capture for n seconds to prevent hiccups
- Can capture with custom naming rulesets and trace channels (UE Insights trace only)

Please refer to Wiki pages for more information

- [Home](https://github.com/excalith/ue-batch-profiler/wiki)
- [Usage](https://github.com/excalith/ue-batch-profiler/wiki/Usage)
- [Commands](https://github.com/excalith/ue-batch-profiler/wiki/Commands)
- [Settings](https://github.com/excalith/ue-batch-profiler/wiki/Settings)
