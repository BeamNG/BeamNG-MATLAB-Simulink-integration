# Introduction

**BeamNG.tech Support for MATLAB and Simulink** is a MATLAB toolbox that allows you to connect [MathWorks](https://www.mathworks.com/) products with [BeamNG.tech](https://beamng.tech/). It comes with two components:


## MATLAB Bridge

**MATLAB Bridge** allows you to control BeamNG from MATLAB scripts, for example to set up scenarios or to query vehicle information. This uses [BeamNGpy](https://github.com/BeamNG/BeamNGpy) leveraging MATLAB's [ability to call Python libraries](https://www.mathworks.com/products/matlab/matlab-and-python.html) under the hood.

```{mermaid}
graph LR;
  ml(MATLAB)<-->py(BeamNGpy)<-->bng(BeamNG.tech);
```

## Simulink Bridge

**Simulink Bridge** allows you to set up a co-simulation between a Simulink model and a BeamNG vehicle. It comes with 2 options:

* S-function block: Uses Simulink's S-function mechanism to communicate to BeamNG via the UDP protocol.

  ```{mermaid}
  graph LR;
    sl(Simulink)<-->sf(S-function)<-->|UDP|bng(BeamNG.tech);
  ```

* FMU block according to the [FMI Standard](https://fmi-standard.org/): Uses Simulink's FMU block with an exported FMU by BeamNG to communicate to BeamNG via the UDP protocol. Both FMI 2.0 and FMI 3.0 versions are available.

  ```{mermaid}
  graph LR;
    sl(Simulink)<-->fmu(FMU)<-->|UDP|bng(BeamNG.tech);
  ```

The **MATLAB Bridge** and the **Simulink Bridge** can be used independently from each other and have different scopes. However, they can be also used together, see [Combined Example](combined.md).
