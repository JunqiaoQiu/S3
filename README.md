# Scalability-Sensitive Speculation framework for FSM parallelization

**S3**, a scalability-sensitive speculation framework for FSM parallelization.

For any given FSM, **S3** can automatically characterize its properties and analyze its scalability, hence guide speculative paralleilization towards the optimal performance more efficient use of computing resources. More details can be found in our paper [1].

## Features
- **S3** is implemented in C++ and leverages Pthread for multi-threading, and uses  our previous MicroSpec Library. 

- There are 3 major components in **S3**, including: 
	1. An offline architecture property collector;
	2. An offline FSM property collector for profiling state convergence properties;
	3. a runtime controller that implements the scalabiltiy models;

- To ensure the correctness, a verifier is provided, to measure and collect the average performance of the given FSM on different configurations.

## Installation

### Obtain Source Code
You can clone this repository by using git:

```sh
git clone https://github.com/JunqiaoQiu/S3Repo.git
```
### Install and Build

The requirements for building **S3** include: `cmake2.8+` and `gcc4.9+`. To build it:

```sh
mkdir build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

Since **S3** needs supports from another project **MicroSpec**, we have applied automatically `git clone` for this external repo, but if you want to manage it by yourself, you can download it using your own way before build **S3**.

## Usage

Currently **S3** can be used as a library, which can feed the analysis results into the speculative FSM parallelization setting at runtime through the provided APIs. Here an simple example shows how to use **S3**. 

```cpp
S3RunTimeController* objController = new S3RunTimeController(numThreads, testLength);
objController->startOfflineProfile(tableObject, InputLibFile, mappingRule);
objController->startModelConstruction("M1+");
printf("The optimal configuration %d\n", objController->getOptConfiguration());
printf("The optimal configuration %d\n", objController->getOptPerformance());

```

At the same time, a command line tool is also provided for analyzing FSM benchmark scalability. Fore more details, you can check this tool **_S3_** with the --help option. 

## Reference
[1] Qiu J, Zhao Z, Wu B, Vishnu A, Song SL. Enabling scalability-sensitive speculative parallelization for fsm computations. InProceedings of the International Conference on Supercomputing 2017 Jun 14 (p. 2). ACM.

