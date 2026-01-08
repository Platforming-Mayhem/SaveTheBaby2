#!/bin/bash
cd "$(dirname "$0")/bin" && rm -rf * && cmake .. && make Components
