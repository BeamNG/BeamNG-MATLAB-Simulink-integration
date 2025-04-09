# Developing Guidelines

These guidelines are for the developers of this toolbox.

## Guidelines

We should follow the following development guidelines:

* [MATLAB Toolbox Best Practices](https://github.com/mathworks/toolboxdesign)
* [Conncecting GitHub Releases to File Exchange](https://www.mathworks.com/matlabcentral/content/fx/about.html#Why_GitHub)
* Follow [Semantic Versioning](https://semver.org/) for version numbers
* We don't use MATLAB's documentation system for publishing the toolbox documentation but host a versioned website generated with Sphinx online
* Work on `dev` branch, merge to `main` only on release

## Releasing a new toolbox version

Follow the following steps to release a new version:

* Update the `ToolboxVersion` in `project.m`
* Add an entry for the new version in `CHANGELOG.rst`
* In MATLAB, run `make_release` to package the `.mltbx` file in the `release` folder
* Commit your changes to the `dev` branch (although it is a derived file, we will commit the `.mltbx` file so that it can be picked up as release artifact) and push `dev` to GitLab
* Merge branch `dev` into `main`
* Push `main` to GitLab, it will trigger rebuild of documentation and if the version number is new it will create a tag and a release
* Fetch tags with `git fetch origin --tags`
* Push `main` and tags to GitHub with:
  ```
  git push github main
  git push github --tags
  ```
  ~~It will trigger creating the release on GitHub. This new release will automatically create a new version on Matlab File Exchange.~~ [not yet implemented, instead do these steps manually]
* Check if the [Documentation](https://documentation.beamng.com/api/matlab-simulink/) has the new version, check [Releases](https://github.com/BeamNG/BeamNG-MATLAB-Simulink-integration/releases) if the release is there, check [File Exchange](https://www.mathworks.com/matlabcentral/fileexchange/166246-beamng-tech-support-for-matlab-and-simulink) if the version was published
