import os
from PIL import Image
import numpy as np
from torch.utils.data import Dataset

class CustomDataset(Dataset):
    def __init__(self, X, y, transform=None):
        random_shuffle = np.random.choice(len(X), len(X), replace=False)
        self.samples = np.array(X.copy(), dtype='object')[random_shuffle]
        self.labels = np.array(y.copy())[random_shuffle]
        self.transform = transform

    def __len__(self):
        return len(self.samples)
    
    def __getitem__(self, idx):
        sample = self.samples[idx]
        label = self.labels[idx]
        if self.transform:
            sample = self.transform(sample)
        return sample, label
    
def make_dataset(folder, labels):
    X = []
    y = []
    for label in labels:
        cur_dir = str(folder) + '/' + str(label)
        for i, img in enumerate(os.listdir(cur_dir)):
            image = Image.open(str(cur_dir) + "/{}".format(img)).resize((128, 128))
            X.append(image)
            y.append(int(label))
    return X, np.array(y)