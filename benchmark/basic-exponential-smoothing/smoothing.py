alpha = 0.05
accumulator = 0

for i in range(1000000):
    accumulator = (alpha * i) + (1.0 - alpha) * accumulator

