# Scalability-Sensitive Speculation framework for FSM parallelization

**S3** is a scalability-sensitive speculation framework for FSM parallelization.

For any given FSM, **S3** can automatically characterize its properties and analyze its scalability, hence guide speculative paralleilization towards the optimal performance more efficient use of computing resources. More details can be found in our paper.

## Features
- **S3** is implemented based on the OptSpec library and our previous MicroSpec project. It is implemented in C++ and leverages Pthread for multi-threading. 

- There are 3 major components in **S3**, including: 
	1. An offline architecture property collector;
	2. An FSM property collector for profiling state convergence properties;
	3. a runtime controller that implements the scalabiltiy models;

- To ensure the correctness, a verifier is provided, to measure and collect the average performance of given FSM on different configurations.

## Installation

### Source code

### Install and Build

## Usage

Currently **S3** can be used as a library, which can feed the analysis results into the speculative FSM parallelization setting at runtime through the provide APIs. 

At the same time, we also construct a linux command-line tool, which will provide the scalability analysis of a given FSM, and verify the correctness by comparing with the ground truth. 

