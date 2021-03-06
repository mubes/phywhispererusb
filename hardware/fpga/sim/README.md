# Verilog Test Enviroment
This directory contains scripts and testbench files to verify the
PhyWhisperer FPGA design in simulation.


## Requirements
* [Icarus Verilog](http://iverilog.icarus.com)
* Python
* Make
* optional: [gtkwave](http://gtkwave.sourceforge.net)
* **no commercial tools!**

## Files
* `tb_pw.v`: Verilog testbench
* `regress.py`: regression script, defines testcases and launches
  simulations via makefile
* `makefile` 

## Example Usage
* `regress.py --list`: lists available testcases.
* `regress.py --runs 10`: run 10 iterations of the regression.
* `regress.py --runs 10 --tests stream`: only run tests whose name contains 'stream'.
* `regress.py --test short_timestamps --seed 13`: run a single testcase with a specific seed.


## Testcases
Testcases are defined in regress.py via `tests`, a list of dictionaries.
Each list element defines a testcase. Each testcase should have `name`,
`description`, and `frequency` keys. The first two are self-explanatory; the
`frequency` key value determines how often a testcase is run in the
regression:

* `frequency = 0`: the test is skipped;
* `frequency = 1`: the test is run at each iteration of the regression;
* `frequency = x`: the test is run once every x iterations.

The frequency of testcases has been set to balance the time it takes to run
a regression. Some testcases take less than 1 second to run while others can
take several minutes. Ideally, a regression with 10 iterations should take
around 10 minutes. 

The rest of the testcase attributes are what really define a testcase.
Before diving into that, let's first explain what the testbench does at a
high level:

1. Send random USB events which have no effect.
2. Send USB events which trigger the FPGA's pattern match logic.
3. Send random USB events which will be captured by the FPGA.
4. Either concurrently with step 3, or after step 3 is done, read the
   captured events from the FPGA (and verify the captured data for
   correctness!).
5. (optionally) Rinse and repeat.

A 'USB event' is a a change on the monitored USB front-end signals, with or
without `rxvalid` being set (`rxvalid` indicates a valid received data
byte). USB events also include timestamp events generated by the FPGA
itself, in order to accurately record the time between USB events received
from the USB3500 chip.

The testbench uses a **constrained random verification** approach: the FPGA
stimulus is randomized, but within certain constraints so that (a) the
stimulus is 'legal'; (b) the testbench knows the expected FPGA response;
(c) 'useful' testcases can be created, where 'usefulness' is loosely defined
as the ratio of testcase complexity and runtime to test coverage obtained.

Randomization is controlled by the following parameters, which you can see
defined for each testcase in regress.py (most testcases don't define every
parameter because each parameter has a default value):
* `PRETRIG_MAX`: number of USB events generated prior to the USB events
  which will trigger the pattern match.
* `PATTERN_MIN`, `PATTERN_MAX`: the size of pattern match (in bytes) is
  randomized within this range.
* `NUM_EVENTS`: number of USB events to be captured by the FPGA.
* `NUM_REPEATS`: number of times that the entire test sequence is repeated
  (see step #5 above, 'rinse & repeat')
* `MIN_DELAY`, `MAX_DELAY`: delay between successive USB events.
* `DELAY_MODE`:
   * 0: the delay between successive USB events is randomized between
     `MIN_DELAY` and `MAX_DELAY`; 
   * 1: the delay is randomly chosen to be either `MIN_DELAY` or `MAX_DELAY`
     (useful to simulate bursty traffic).
* `VALID`: percentage (from 0 to 100) of USB events that have rxvalid set.
* `TRIGGER_ENABLE`: if set, a trigger will be issued after a pattern match.
* `TRIGGER_DELAY_MIN`, `TRIGGER_DELAY_MAX`: after a pattern match, delay
  until trigger is asserted.
* `TRIGGER_WIDTH_MIN`, `TRIGGER_WIDTH_MAX`: width of trigger.
* `READ_CONCURRENTLY`:
   * 0: wait for the capture to be complete, then read everything in a
     single burst read;
   * 1: testbench starts to read captured data as soon as it's available,
     one word at a time. 
* `STREAM_MODE`:
   * 0: wait for captured data to be available before reading;
   * 1: read captured data continuously, even if data isn't available. 
* `VERBOSE`: always set.
* `TIMEOUT`: global timeout.
* `TOP`: specify top-level Verilog file. Allows a testcase to use a different
  Verilog testbench altogether (this is used by the USB autodetect testcase).


## Randomization
Many of the above parameters set range limits for things that are randomized
(e.g. minimum and maximum delay). By default, different randomization is
used every time a testcase is run. However, to repeat a previous
randomization (e.g. for debugging a failing testcase), the SEED parameter
can be set via the `--seed` command-line switch.


## Simulation Output
Every test run generates a numbered logfile in the `results` directory.
Every test also generates a waveform file (`results/tb.fst`, can be opened
with gtkwave), however only the last generated waveform file is kept.


## What's Missing
* Formal verification (e.g. using
[SymbiYosys](https://github.com/YosysHQ/SymbiYosys)) could be good to add.

* Functional coverage and code coverage are tools that can be helpful in
  determining whether the test environment is fully exercising the target
  design; unfortunately these capabilities are only offered by commercial
  tools.

* Assertions can simplify and add robustness to the verification
  environment; they are also not fully supported by open source simulators
  ([Verilator](https://www.veripool.org/projects/verilator/wiki/Intro)
  appears to have limited support).

