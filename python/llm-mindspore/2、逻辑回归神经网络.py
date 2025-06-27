import numpy as np
import mindspore
import mindspore.nn as nn
import matplotlib.pyplot as plt
from mindspore.dataset import GeneratorDataset

# 生成数据
x = np.random.randint(-20, 20, 450)
x.sort()
y_0 = np.zeros(150)
y_1 = np.ones(150)
y_2 = np.zeros(150)
y = np.concatenate([y_0, y_1, y_2])

# 转换tensor
x_ms_tensor = mindspore.Tensor(x, dtype=mindspore.float32).reshape(-1, 1)
y_ms_tensor = mindspore.Tensor(y, dtype=mindspore.float32).reshape(-1, 1)

# 散点图
plt.scatter(x_ms_tensor, y_ms_tensor)

# dataset 和 dataloader
dataset = GeneratorDataset(list(zip(x_ms_tensor, y_ms_tensor)), ['x', 'y'])
dataloader = dataset.batch(batch_size=10)


# 定义逻辑回归模型
class LinearModel(nn.Cell):
    def __init__(self):
        super().__init__()
        self.input = nn.Linear(1, 2)
        self.output = nn.Linear(2, 1)
        self.sigmoid = nn.Sigmoid()  # 实例化

    def construct(self, x):
        y = self.sigmoid(self.input(x))
        return self.output(y)


# 初始化模型
model = LinearModel()
optimizer = nn.SGD(model.trainable_params(), learning_rate=0.1)
criterion = nn.MSELoss()

# 定义训练网络
net_with_loss = nn.WithLossCell(model, criterion)
tran_net = nn.TrainOneStepCell(net_with_loss, optimizer)

# 训练模型
step = 100
for _ in range(step):
    for x_batch, y_batch in dataloader:
        tran_net(x_batch, y_batch)

y_final_predict = model(x_ms_tensor)
# 可视化
plt.plot(x, y_final_predict.asnumpy())
plt.show()
