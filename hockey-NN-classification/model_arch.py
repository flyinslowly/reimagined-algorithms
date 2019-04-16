import torch.nn as nn

class CNN(nn.Module):
    def __init__(self):
        super(CNN,self).__init__()
        self.cnn1 = nn.Conv2d(in_channels=3, out_channels=5, kernel_size=3, stride=1, padding=1)
        self.batchnorm1 = nn.BatchNorm2d(5)
        self.relu = nn.ReLU()
        self.maxpool1 = nn.MaxPool2d(kernel_size=2)
        self.cnn2 = nn.Conv2d(in_channels=5, out_channels=8, kernel_size=5, stride=1, padding=2)
        self.batchnorm2 = nn.BatchNorm2d(8)
        self.maxpool2 = nn.MaxPool2d(kernel_size=4)
        self.fc1 = nn.Linear(in_features=2048, out_features=2)
        
    def forward(self, X):
        out = self.cnn1(X)
        out = self.batchnorm1(out)
        out = self.relu(out)
        out = self.maxpool1(out)
        
        out = self.cnn2(out)
        out = self.batchnorm2(out)
        out = self.relu(out)
        out = self.maxpool2(out)
        
        #print(out.shape)
        
        out = out.view(-1, 2048)
        
        out = self.fc1(out)
        return out