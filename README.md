# gipLibgizmo

GlistEngine plugin providing 3D transform gizmos (translate/rotate/scale handles) for selected objects at runtime, built on top of [LibGizmo](https://github.com/CedricGuillemet/LibGizmo) (MIT licensed).

LibGizmo's source (`inc/`, `src/libgizmo/`) is vendored directly into `libs/include` and `libs/src` — see `libs/LICENSE` for its MIT license text. A few minimal fixes were made on top of the vendored code: NaN-guarding on the transform matrices (`CheckMatrixNaN` in `GizmoTransform.h`) and two uninitialized-enum fixes in `GizmoTransformMove`/`GizmoTransformScale`. `gipLibgizmo` wraps it with a GlistEngine-native API: it reads the active camera's matrices from `gRenderer`, draws the gizmo via `gRenderer`/`gVbo`, and forwards mouse input to it via the standard `gBasePlugin` event hooks.

Runtime-only for now: the gizmo is drawn as an in-scene overlay while the app is running (GlistEngine has no separate editor mode). A dedicated editor mode is a possible future phase. Android support is not yet implemented.

- Windows developers should not forget to add
```
${workspace_loc}\..\..\..\..\glistplugins\gipLibgizmo\libs\bin
```
directory to the GlistApp project's PATH list.
(Project->Properties->C/C++ Build->Environment->PATH)
