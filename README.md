## GenDP: A Dynamic Programming Acceleration Framework for Genome Sequencing Analysis

### CPU Baselines

The table below shows the CPU system configuration and the runtime(second) for each kernel.

| CPU                                          | SIMD Flag | Operatin System       | Threads | BSW    | Chain | PairHMM | POA   |
| -------------------------------------------- | --------- | --------------------- | ------- | -----  | ----- | ------- | ----- |
| Intel(R) Xeon(R) Platinum 8380 CPU @ 2.30GHz | AVX512    | CentOS Linux 7 (CORE) | 80      |  |  |    |  |
| Intel(R) Xeon(R) Gold 6326 CPU @ 2.90GHz     | AVX512    | Ubuntu 20.04.5 LTS    | 32      | 0.0984 | 0.473 | 0.678   | 7.389 |
| Intel(R) Xeon(R) CPU E5-2697 v3 @ 2.60GHz    | AVX2      | CentOS Linux 7 (CORE) | 28      | 0.196  | 2.351 | 2.13    | 10.58 |


### GPU Baselines

The table below shows the GPU system configuration and the runtime(second) for each kernel.

| GPU               | CUDA | BSW   | Chain | PairHMM | POA   |
| ----------------- | ---- | ----- | ----- | ------  | ----- |
| NVIDIA RTX A100   | 11.2 |  |  |    |   |
| NVIDIA RTX A6000  | 12.0 | 0.012 | 0.339 | 0.572   | 3.70  |
| TITAN Xp          | 10.2 | 0.020 | 0.747 | 0.915   | 11.17 |
