# Installation

To make MATLAB and Simulink work with BeamNG.tech, you have to install and configure several software components first:
* MATLAB and optionally Simulink
* Python and the BeamNGpy package
* BeamNG.tech
* *BeamNG.tech Support for MATLAB and Simulink* (this toolbox)

The following sections describe the installation process in detail.

```{note}
Before continuing, check [Compatibility](compatibility.md) to make sure you install the correct versions.
```

## Install BeamNG.tech

The first step is to install [BeamNG.tech](https://beamng.tech/). Check the website how to
acquire a license, and follow the provided instructions to download and install the software.

It is also recommended to create an environment variable `BNG_HOME` pointing to the folder containing the
BeamNG.tech executable. With this your scripts become simpler and more portable, because you don't need to
hardcode this path in you scripts. To create a new environment variable on **Windows**, perform the following steps:
1. Click **Start** and enter *environment*
2. Click on **Edit environment variables for your account** (or if you have administrator privileges, you can also choose **Edit the system environment variables** and then **Environment Variables...**)
3. Click **New...** and enter the following information:
   
   ![new_environment_variable](pictures/new_environment_variable.png)

   The variable value is the path to the **folder** where the file **BeamNG.drive.exe** is located.
4. Click OK

## Install and set up Python

Once you have BeamNG.tech installed you can proceed installing [Python](https://www.python.org/). 
Install a
[Python version compatible with your MATLAB version](https://nl.mathworks.com/support/requirements/python-compatibility.html). During installation, make sure you check **Add Python to PATH**, so that it is recognized as an available program.

```{tip}
If you forgot to tick the adding to PATH option during installation, you can either re-run the Python installer and check it this time, or you can add the following two entries to your in `PATH` environment variable (see how to set environment variables above): `path\to\python` and `path\to\python\Scripts`.
```

## Install *beamngpy*

Having a Python
environment ready, you can proceed installing [BeamNGpy](https://github.com/BeamNG/BeamNGpy) on it, when doing
so keep in mind to comply with the
[compatibility table
between BeamNGpy and BeamNG.tech](https://github.com/BeamNG/BeamNGpy#compatibility).

You can install BeamNGpy using `pip`. Open a Command Prompt or Terminal and type the following to install the latest version of BeamNGpy.

```
pip install beamngpy
```

To install a different version (e.g. `1.30`), type

```
pip install beamngpy==1.30
```

## Install *BeamNG.tech Support for MATLAB and Simulink*

The simplest way is to install the latest version via the MATLAB Add-On Manager ([Option 1](#option-1-install-latest-version-via-add-on-manager)). However, you can also manually install any version of the toolbox, including older versions ([Option 2](#option-2-manually-install-any-desired-version)).

### Option 1: Install latest version via Add-On Manager

TODO

![BeamNG-MATLAB-Simulink-integration_Toolbox](media/BeamNG-MATLAB-Simulink-integration_Toolbox.png)

### Option 2: Manually install any desired version

TODO

## Connect MATLAB to Python

The last step that you need to perform is to connect MATLAB to your new
python environment. 
This can be easily done using the `setup_beamngpy` function. 
For example, if you have a conda python environment you just need to call
this function from the command window with a similar input:

```matlab
setup_beamngpy('C:\Users\<username>\miniconda3\envs\<env-name>\python.exe')
```

Now you are ready to use BeamNGpy through MATLAB. If you have Simulink installed, you can also use the Simulink co-simulation functionalities now.

## Test if it works

TODO: minimal example to test

Make sure that python engine is connected to your MATLAB engine

```matlab
pe = pyenv;
pe.Version % This should return your python version linked with your MATLAB
pyenv(ExecutionMode="OutOfProcess") % This should return paths for your python executable, library, etc.
```
