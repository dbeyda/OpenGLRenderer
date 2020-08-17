# OpenGL Renderer
> A 3D renderer developed as an introduction to real-time rendering.

 This project is a 3D renderer developed as an introduction to real-time rendering.
 It was developed mainly during PUC-Rio's Real-Time Rendering class, and then further improved over winter recess.
 Credits for resources that made this possible are at the bottom of the page.
 
 ## Features:
 * Load `.obj` files for meshes.
 * Load `.png` and `.jpg` for textures.
 * Load `.mtl` for material properties.
 * Importing diffuse, specular and ambient maps.
 * Support for **multiple lights and models**.
 * Basic camera movements.
 * Forward rendering.
 * **Spotlights**.
 * **Phong Lighting Model**, with diffuse, speccular and ambient lighting.
 * **Shadow Mapping** with soft shadows using jittered offsets, as seen [here](https://developer.nvidia.com/gpugems/gpugems2/part-ii-shading-lighting-and-shadows/chapter-17-efficient-soft-edged-shadows-using).
 * **Bump Mapping**, with vertices tangents calculated on mesh import.
 * **Depth of Field** that can be toggled dinamically during runtime.

## Images

OS X & Linux:

```sh
npm install my-crazy-module --save
```

Windows:

```sh
edit autoexec.bat
```

## Usage example

A few motivating and useful examples of how your product can be used. Spice this up with code blocks and potentially more screenshots.

_For more examples and usage, please refer to the [Wiki][wiki]._

## Development setup

Describe how to install all development dependencies and how to run an automated test-suite of some kind. Potentially do this for multiple platforms.

```sh
make install
npm test
```

## Release History

* 0.2.1
    * CHANGE: Update docs (module code remains unchanged)
* 0.2.0
    * CHANGE: Remove `setDefaultXYZ()`
    * ADD: Add `init()`
* 0.1.1
    * FIX: Crash when calling `baz()` (Thanks @GenerousContributorName!)
* 0.1.0
    * The first proper release
    * CHANGE: Rename `foo()` to `bar()`
* 0.0.1
    * Work in progress

## Meta

Your Name – [@YourTwitter](https://twitter.com/dbader_org) – YourEmail@example.com

Distributed under the XYZ license. See ``LICENSE`` for more information.

[https://github.com/yourname/github-link](https://github.com/dbader/)

## Contributing

1. Fork it (<https://github.com/yourname/yourproject/fork>)
2. Create your feature branch (`git checkout -b feature/fooBar`)
3. Commit your changes (`git commit -am 'Add some fooBar'`)
4. Push to the branch (`git push origin feature/fooBar`)
5. Create a new Pull Request

<!-- Markdown link & img dfn's -->
[npm-image]: https://img.shields.io/npm/v/datadog-metrics.svg?style=flat-square
[npm-url]: https://npmjs.org/package/datadog-metrics
[npm-downloads]: https://img.shields.io/npm/dm/datadog-metrics.svg?style=flat-square
[travis-image]: https://img.shields.io/travis/dbader/node-datadog-metrics/master.svg?style=flat-square
[travis-url]: https://travis-ci.org/dbader/node-datadog-metrics
[wiki]: https://github.com/yourname/yourproject/wiki
