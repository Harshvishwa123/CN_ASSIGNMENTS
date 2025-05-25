
# 🖧 Computer Networks Assignments – CSE232

---

## 📁 Repository Overview

This repository contains completed assignments from the Computer Networks course (CSE232), including:

- 🔧 Command-line based network exploration
- 🔐 Firewall and routing using `iptables`
- 📶 NS-3 based network simulations and congestion control
- 🧵 Multi-threaded client-server implementation in C

---

## 📄 Assignment 1: Network Exploration Using CLI Tools

This assignment involved using Linux command-line utilities to analyze:

- 📡 Local vs Public IPs via `ip` and `whatismyip.com`
- 🔄 IP reassignment using `ip` commands
- 🔌 TCP connection between host and VM
- 🧭 DNS queries with `nslookup`
- 🚀 Traceroute & Ping latency comparison
- 🔒 Packet filtering using `iptables`

📄 File: `2022205-Assignment1.pdf`

---

## 📄 Assignment 2: TCP Client-Server Programming in C

A multi-threaded TCP server was implemented to return the top 2 CPU-consuming processes using `/proc/[pid]/stat`. Two types of clients were created:

### 🧠 Server Features:
- Multi-threaded using `pthread`
- Reads `/proc` filesystem to find CPU-heavy processes
- Responds to client requests over port `8080`

### 🧠 Client Programs:
- `client.c`: Creates multiple threads, each making a connection
- `client2.c`: Sequential connections and response printing

📂 Files:
- `server1.c`
- `client.c`
- `client2.c`

---

## 📄 Assignment 3: Packet Filtering & NAT with iptables

This assignment demonstrates the configuration and filtering of packets using `iptables` and manual route configuration in a multi-VM setup.

### 🔑 Key Tasks:
- Configured static IPs and interfaces on 4 VMs
- Enabled routing and tested with ping, TCP, and UDP
- Used `iptables` rules to filter specific traffic
- Setup NAT and load balancing using SNAT, DNAT, and probabilistic routing

📄 File: `2022205_Assignment3.pdf`

---

## 📄 Assignment 4: TCP Congestion Control Analysis using ns-3

This assignment focused on simulating network behavior using **ns-3** to measure:

- 📊 Theoretical vs Actual Throughput
- 📉 Bandwidth-delay product and congestion window
- 📈 Queue delays and packet drops
- 🧪 Effects of varying BDP and queue size on throughput

📄 File: `CN A4.pdf`

---

## ✅ How to Run Assignment 2 (TCP Server-Client in C)

1. Compile the server and client files:
```bash
gcc server1.c -o server -lpthread
gcc client.c -o client -lpthread
gcc client2.c -o client2
```

2. Start the server:
```bash
./server
```

3. Run clients:
```bash
./client 3         # runs 3 parallel client threads
./client2 3        # runs 3 sequential client requests
```

---

## 📚 References

- Linux Man Pages
- `iptables` official documentation
- ns-3 Simulator
- `/proc` Linux filesystem
