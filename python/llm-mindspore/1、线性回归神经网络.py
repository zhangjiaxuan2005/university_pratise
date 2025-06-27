import numpy as np
import matplotlib.pyplot as plt
import mindspore
import mindspore.nn as nn
from mindspore.dataset import GeneratorDataset

# 生成数据
x = np.random.randint(1, 100, 100)
y = np.array([i * 10 + np.random.randint(100) for i in x])

# 数据归一化
x = (x - x.min()) / (x.max() - x.min())
y = (y - y.min()) / (y.max() - y.min())

# 转换成mindspore的tensor
x_ms_tensor = mindspore.tensor(x, dtype=mindspore.float32).reshape(-1, 1)
y_ms_tensor = mindspore.tensor(y, dtype=mindspore.float32).reshape(-1, 1)

# 数据可视化
plt.scatter(x_ms_tensor, y_ms_tensor)

# dataset 和 dataloader
dataset = GeneratorDataset(list(zip(x_ms_tensor, y_ms_tensor)), ['x', 'y'])
dataloader = dataset.batch(batch_size=10)


# 定义线性回归模型
class LinearRegression(nn.Cell):
    def __init__(self):
        super().__init__()
        self.linear = nn.Linear(1, 1)

    def construct(self, x):
        return self.linear(x)


# 初始化模型
model = LinearRegression()
criterion = nn.MSELoss()
optimizer = nn.SGD(model.trainable_params(), learning_rate=0.1)

# 定义训练网络
net_with_loss = nn.WithLossCell(model, criterion) # 前向传播和损失函数
train_net = nn.TrainOneStepCell(net_with_loss, optimizer)# 反向传播

# 训练模型
step = 100

for _ in range(step):
    for x_batch, y_batch in dataloader:
       train_net(x_batch, y_batch)

# 预测
y_final_predict = model(x_ms_tensor)

# 可视化
plt.plot(x, y_final_predict.asnumpy())
plt.show()
