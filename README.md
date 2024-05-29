# EPICS Base - the central core of a control system toolkit

The Experimental Physics and Industrial Control System (EPICS) comprises a set of software components and tools that can be used to create distributed control systems.

- [Website](https://epics-controls.org/)
- [Documentation](https://docs.epics-controls.org/en/latest/index.html)
- [Repository](https://git.launchpad.net/epics-base)
- [Mailing List (Tech Talk)](https://epics.anl.gov/tech-talk/)

## Quick Install

These are quick instructions for working on mac on linux. See [Installation Overview](https://docs.epics-controls.org/en/latest/getting-started/installation.html) for more detailed instructions.

### Prerequisites

- libreadline
- make
- c++

### Install

Clone and make:

```bash
mkdir $HOME/EPICS
cd $HOME/EPICS
git clone --recursive https://github.com/epics-base/epics-base.git
cd epics-base
make
```

Add the binaries to your shell `$HOME/.profile` or into `$HOME/.bashrc`

```bash
export EPICS_BASE=${HOME}/EPICS/epics-base
export EPICS_HOST_ARCH=$(${EPICS_BASE}/startup/EpicsHostArch)
export PATH=${EPICS_BASE}/bin/${EPICS_HOST_ARCH}:${PATH}
``` 

## Run a server

Run `softIoc` and, if everything is ok, you should see an EPICS prompt.

```bash
softIoc
epics>
```

For a more detailed example see [Creating an IOC Application](https://docs.epics-controls.org/en/latest/getting-started/creating-ioc.html).