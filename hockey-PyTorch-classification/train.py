import torch

def model_train(model, optimizer, loss_fn, train_load, test_load, epochs):
    iter = 0
    for epoch in range(epochs):
        for i, (images, labels) in enumerate(train_load):
            iter += 1
            images = images.reshape((-1, 3, 128, 128))
            #print(images.shape)
            
            optimizer.zero_grad()
            outputs = model(images)
            
            labels = labels.view(-1)

            #print(outputs.shape, labels.shape)
            loss = loss_fn(outputs, labels)
            
            loss.backward()
            optimizer.step()

        correct = 0
        total = 0
        for images, labels in test_load:
            labels = labels.reshape(-1)
            images = images.reshape((-1, 3, 128, 128))
            outputs = model(images)
            _, predicted = torch.max(outputs.data, 1)
            total += labels.size(0)
            correct += (predicted==labels).sum()
        accuracy = 100 * correct/total
        
        
        print(epoch, "Done!", 'Train iteration: {}, Train Loss: {}, Test Accuracy {}%'.format(iter, loss.data[0], accuracy))