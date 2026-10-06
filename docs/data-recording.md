# Data recording

The recorder makes captured measurements available for offline processing.

## Binary format

The project README defines a header containing:

- `uint32_t sample_rate`
- `uint32_t num_samples`

followed by interleaved `float32` voltage/current samples.

Conceptually:

```text
V0, I0, V1, I1, V2, I2, ...
```

## Python example

```python
import numpy as np

with open("data.bin", "rb") as f:
    sample_rate = np.fromfile(f, dtype=np.uint32, count=1)[0]
    num_samples = np.fromfile(f, dtype=np.uint32, count=1)[0]
    data = np.fromfile(f, dtype=np.float32).reshape(-1, 2)

voltage = data[:, 0]
current = data[:, 1]
time = np.arange(num_samples) / sample_rate
```

This makes recorded measurements straightforward to inspect with NumPy or other scientific tooling.
