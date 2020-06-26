Intel(R) SGX Switchless ECALL demo application
================================================

# Overview
This application demonstrates how to design Intel(R) SGX Switchless ecall interfaces.

# Background
Intel(R) SGX provides trusted execution environment. User's confidential workload can execute in isolated environment (named enclave) with confidentiality and integrity protection. Application calls enclave interface through ECALL instruction. It's measured that ECALL instruction is time-consuming, about 6 times of system call latency. One popular method to migate ECALL latency is through switchless ECALL. Generally, this is an aync function call mechnism, it launches some working threads inside enclave, which monitors function requests from normal application. The function requests are pooled in a buffer, which is shared with enclave runtime. When application needs to do switchless ecall, it places function requests in the request queue and notify trusted part to hanle the requests. Enclave working thread would read and process the requests. This application provides a demo. how to design such interface.
Intel(R) SGX SDK have provided runtime library to support switchless ECALL/OCALL. This application can help you understand the Overall design strategy.

# Build Instruction
* Hardware Pre-requirements:
   SGX capable hardware platform with Intel(R) SGX Flexible Launch Control support. SGX is enabled in BIOS.

* Software Pre-requirements:
1. To build the project from source code, you need to install Intel(R) SGX SDK.
2. To execute the demo. application, you need to
   * install Intel(R) SGX driver and platform software.
   Please refer to related SGX documents to know how to do this.

* Supported Operation systems:
  - Ubuntu 16.04 LTS Desktop 64bits
  - Ubuntu 18.04 LTS Desktop 64bits

* Build steps:
1. in source code root directory, run "make"
2. when build complete, it would generate two output folders:
   "bin" subfolder - includes demo. application
   "lib" subfolder - include trusted run time library to establish secure session.

# Execution steps:
------
You can run demo. application with below stpes:
1. in "bin" sub-folder, execute "./app"

# Contract:
If you have any questions or comments, you can send mail to antoniodanhu@gmail.com.

