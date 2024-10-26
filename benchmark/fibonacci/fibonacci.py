trailing = 0
leading = 1

for _ in range(1000000):
    trailing, leading = leading, trailing + leading

