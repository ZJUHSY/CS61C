import numpy as np
import cPickle as pickle
from time import time
from classifier import Classifier
from util.layers import *
from util.dump import *

""" STEP3: Build Deep Convolutional Neural Network """

class CNNClassifier(Classifier):
  def __init__(self, D, H, W, K, iternum):
    Classifier.__init__(self, D, H, W, K, iternum)

    """ 
    Layer 1 Parameters (Conv 32 x 32 x 16) 
    K = 16, F = 5, S = 1, P = 2
    weight matrix: [K1 * D * F1 * F1]
    bias: [K1 * 1]
    """
    K1, F1, self.S1, self.P1 = 16, 5, 1, 2
    self.A1 = 0.01 * np.random.randn(K1, D, F1, F1)
    self.b1 = np.zeros((K1, 1))
    H1 = (H - F1 + 2*self.P1) / self.S1 + 1
    W1 = (W - F1 + 2*self.P1) / self.S1 + 1

    """ 
    Layer 3 Parameters (Pool 16 x 16 x 16) 
    K = 16, F = 2, S = 2
    """
    K3, self.F3, self.S3 = K1, 2, 2
    H3 = (H1 - self.F3) / self.S3 + 1
    W3 = (W1 - self.F3) / self.S3 + 1
 
    """ 
    Layer 4 Parameters (Conv 16 x 16 x 20) 
    K = 20, F = 5, S = 1, P = 2
    weight matrix: [K4 * K3 * F4 * F4]
    bias: [K4 * 1]
    """
    K4, F4, self.S4, self.P4 = 20, 5, 1, 2
    self.A4 = 0.01 * np.random.randn(K4, K3, F4, F4)
    self.b4 = np.zeros((K4, 1))
    H4 = (H3 - F4 + 2*self.P4) / self.S4 + 1
    W4 = (W3 - F4 + 2*self.P4) / self.S4 + 1

    """ 
    Layer 6 Parameters (Pool 8 x 8 x 20) 
    K = 20, F = 2, S = 2
    """
    K6, self.F6, self.S6 = K4, 2, 2
    H6 = (H4 - self.F6) / self.S6 + 1
    W6 = (W4 - self.F6) / self.S6 + 1

    """ 
    Layer 7 Parameters (Conv 8 x 8 x 20) 
    K = 20, F = 5, S = 1, P = 2
    weight matrix: [K7 * K6 * F7 * F7]
    bias: [K7 * 1]
    """
    K7, F7, self.S7, self.P7 = 20, 5, 1, 2
    self.A7 = 0.01 * np.random.randn(K7, K6, F7, F7)
    self.b7 = np.zeros((K7, 1))
    H7 = (H6 - F7 + 2*self.P7) / self.S7 + 1
    W7 = (W6 - F7 + 2*self.P7) / self.S7 + 1

    """ 
    Layer 9 Parameters (Pool 4 x 4 x 20) 
    K = 20, F = 2, S = 2
    """
    K9, self.F9, self.S9 = K7, 2, 2
    H9 = (H7 - self.F9) / self.S9 + 1
    W9 = (W7 - self.F9) / self.S9 + 1

    """ 
    Layer 10 Parameters (FC 1 x 1 x K)
    weight matrix: [(K6 * H_6 * W_6) * K] 
    bias: [1 * K]
    """
    self.A10 = 0.01 * np.random.randn(K9 * H9 * W9, K)
    self.b10 = np.zeros((1, K))

    """ Hyperparams """
    # learning rate
    self.rho = 1e-2
    # momentum
    self.mu = 0.9
    # reg strength
    self.lam = 0.1
    # velocity for A1: [K1 * D * F1 * F1]
    self.v1 = np.zeros((K1, D, F1, F1))
    # velocity for A4: [K4 * K3 * F4 * F4]
    self.v4 = np.zeros((K4, K3, F4, F4))
    # velocity for A7: [K7 * K6 * F7 * F7]
    self.v7 = np.zeros((K7, K6, F7, F7))
    # velocity for A10: [(K9 * H9 * W9) * K]   
    self.v10 = np.zeros((K9 * H9 * W9, K))
 
    return

  def load(self, path):
    data = pickle.load(open(path + "layer1"))
    assert(self.A1.shape == data['w'].shape)
    assert(self.b1.shape == data['b'].shape)
    self.A1 = data['w']
    self.b1 = data['b'] 
    data = pickle.load(open(path + "layer4"))
    assert(self.A4.shape == data['w'].shape)
    assert(self.b4.shape == data['b'].shape)
    self.A4 = data['w']
    self.b4 = data['b']
    data = pickle.load(open(path + "layer7"))
    assert(self.A7.shape == data['w'].shape)
    assert(self.b7.shape == data['b'].shape)
    self.A7 = data['w']
    self.b7 = data['b']
    data = pickle.load(open(path + "layer10"))
    assert(self.A10.shape == data['w'].shape)
    assert(self.b10.shape == data['b'].shape)
    self.A10 = data['w']
    self.b10 = data['b']
    return 

  def param(self):
    return [
      ("A10", self.A10), ("b10", self.b10),
      ("A7", self.A7), ("b7", self.b7), 
      ("A4", self.A4), ("b4", self.b4), 
      ("A1", self.A1), ("b1", self.b1)] 

  def forward(self, data):
    A1 = self.A1
    b1 = self.b1
    S1 = self.S1
    P1 = self.P1

    F3 = self.F3
    S3 = self.S3

    A4 = self.A4
    b4 = self.b4
    S4 = self.S4
    P4 = self.P4

    F6 = self.F6
    S6 = self.S6

    A7 = self.A7
    b7 = self.b7
    S7 = self.S7
    P7 = self.P7

    F9 = self.F9
    S9 = self.S9

    A10 = self.A10
    b10 = self.b10
    """
    INPUT:
      - data: RDD[(key, (images, labels)) pairs]
    OUTPUT:
      - RDD[(key, (images, list of layers, labels)) pairs]
    """

    """ TODO: Layer1: Conv (32 x 32 x 16) forward """
    L1 = data.map(lambda (k, (x,y)) : (k, (x, [conv_forward(x, A1, b1, S1, P1)], y)))

    """ TODO: Layer2: ReLU (32 x 32 x 16) forward """
    L2 = L1.map(lambda (k, (x, l ,y)) : (k, (x, l + [ReLU_forward(l[0][0])], y)))
    
    """ DOTO: Layer3: Pool (16 x 16 x 16) forward """
    L3 = L2.map(lambda (k, (x, l,y)) : (k, (x, l + [max_pool_forward(l[1], F3, S3)], y)))

    """ TODO: Layer4: Conv (16 x 16 x 20) forward """ 
    L4 = L3.map(lambda (k, (x, l,y)) : (k, (x, l + [conv_forward(l[2][0], A4, b4, S4, P4)], y)))
    """ TODO: Layer5: ReLU (16 x 16 x 20) forward """
    L5 = L4.map(lambda (k, (x, l,y)) : (k, (x, l + [ReLU_forward(l[3][0])], y)))

    """ TODO: Layer6: Pool (8 x 8 x 20) forward """ 
    L6 = L5.map(lambda (k, (x, l,y)) : (k, (x, l + [max_pool_forward(l[4], F6, S6)] ,y)))
    """ TODO: Layer7: Conv (8 x 8 x 20) forward """ 
    L7 = L6.map(lambda (k, (x, l,y)) : (k, (x, l + [conv_forward(l[5][0], A7, b7, S7, P7)] ,y)))

    """ TODO: Layer8: ReLU (8 x 8 x 20) forward """ 
    L8 = L7.map(lambda (k, (x, l,y)) : (k, (x, l + [ReLU_forward(l[6][0])] ,y)))
    """ TODO: Layer9: Pool (4 x 4 x 20) forward """ 
    L9 = L8.map(lambda (k, (x, l,y)) : (k, (x, l + [max_pool_forward(l[7], F9, S9)] ,y)))

    """ TODO: Layer10: FC (1 x 1 x 10) forward """
    L10 = L9.map(lambda (k, (x, l,y)) : (k, (x, l + [linear_forward(l[8][0], A10, b10)] ,y)))

    return L10

  def backward(self, data, count):
    """
    INPUT:
      - data: RDD[(images, list of layers, labels) pairs]
    OUTPUT:
      - Loss
    """
    A1 = self.A1
    b1 = self.b1
    S1 = self.S1
    P1 = self.P1

    F3 = self.F3
    S3 = self.S3

    A4 = self.A4
    b4 = self.b4
    S4 = self.S4
    P4 = self.P4

    F6 = self.F6
    S6 = self.S6

    A7 = self.A7
    b7 = self.b7
    S7 = self.S7
    P7 = self.P7

    F9 = self.F9
    S9 = self.S9

    A10 = self.A10
    b10 = self.b10
    
    #pack values
    layers = data.map(lambda (x,l,y) : l)
    layer1 = layers.map(lambda l : l[0][0])
    X_col1 = layers.map(lambda l : l[0][1])
    layer2 = layers.map(lambda l : l[1])
    layer3 = layers.map(lambda l : l[2][0])
    X_idx3 = layers.map(lambda l : l[2][1])
    layer4 = layers.map(lambda l : l[3][0])
    X_col4 = layers.map(lambda l : l[3][1])
    layer5 = layers.map(lambda l : l[4])
    layer6 = layers.map(lambda l : l[5][0])
    X_idx6 = layers.map(lambda l : l[5][1])
    layer7 = layers.map(lambda l : l[6][0])
    X_col7 = layers.map(lambda l : l[6][1])
    layer8 = layers.map(lambda l : l[7])
    layer9 = layers.map(lambda l : l[8][0])
    X_idx9 = layers.map(lambda l : l[8][1])
    layer10 = layers.map(lambda l : l[9])

    Y = data.map(lambda (x, l, y) : y)
    X = data.map(lambda (x, l, y) : x)

    """ TODO: Softmax Loss Layer """ 
    softmax_back = layer10.zip(Y).map(lambda (l10, Y) : softmax_loss(l10, Y)) 
    L = softmax_back.map(lambda (L, dLdl10) : L).reduce(lambda a,b : a+b)
    dLdl10 = softmax_back.map(lambda (L, dLdl10) : dLdl10)
 
    """ TODO: Layer10: FC (1 x 1 x 10) Backward """
    l10_back = dLdl10.zip(layer9).map(lambda (dLdl10, l9) : linear_backward(dLdl10, l9, A10))
    dLdl9 = l10_back.map(lambda (dLdl9, dLdA10, dLdb10) : dLdl9)
    dLdA10 = l10_back.map(lambda (dLdl9, dLdA10, dLdb10) : dLdA10).reduce(lambda a,b : a+b)
    dLdb10 = l10_back.map(lambda (dLdl9, dLdA10, dLdb10) : dLdb10).reduce(lambda a,b : a+b)

    """ TODO: Layer9: Pool (4 x 4 x 20) Backward """
    # dLdl9, dLdA10, dLdb10 = layer
    dLdl8 = dLdl9.zip(layer8).zip(X_idx9).map(lambda ((dLdl9, layer8), X_idx9) : max_pool_backward(dLdl9, layer8, X_idx9, F9, S9))

    """ TODO: Layer8: ReLU (8 x 8 x 20) Backward """
    dLdl7 = dLdl8.zip(layer7).map(lambda (dLdl8, layer7) : ReLU_backward(dLdl8, layer7))

    """ TODO: Layer7: Conv (8 x 8 x 20) Backward """
    l7_back = dLdl7.zip(layer6).zip(X_col7).map(lambda ((dLdl7, layer6), X_col7) : conv_backward(dLdl7, layer6, X_col7, A7, S7, P7))
    dLdl6 = l7_back.map(lambda (dLdl6, dLdA7, dLdb7) : dLdl6)
    dLdA7 = l7_back.map(lambda (dLdl6, dLdA7, dLdb7) : dLdA7).reduce(lambda a,b : a+b)
    dLdb7 = l7_back.map(lambda (dLdl6, dLdA7, dLdb7) : dLdb7).reduce(lambda a,b : a+b)

    """ TODO: Layer6: Pool (8 x 8 x 20) Backward """
    dLdl5 = dLdl6.zip(layer5).zip(X_idx6).map(lambda ((dLdl6, layer5), X_idx6) : max_pool_backward(dLdl6, layer5, X_idx6, F6, S6))
    
    """ TODO: Layer5: ReLU (16 x 16 x 20) Backward """ 
    dLdl4 = dLdl5.zip(layer4).map(lambda (dLdl5, layer4) : ReLU_backward(dLdl5, layer4))

    """ TODO: Layer4: Conv (16 x 16 x 20) Backward """ 
    l4_back = dLdl4.zip(layer3).zip(X_col4).map(lambda ((dLdl4, layer3), X_col4) : conv_backward(dLdl4, layer3, X_col4, A4, S4, P4))
    dLdl3 = l4_back.map(lambda (dLdl3, dLdA4, dLdb4) : dLdl3)
    dLdA4 = l4_back.map(lambda (dLdl3, dLdA4, dLdb4) : dLdA4).reduce(lambda a,b : a+b)
    dLdb4 = l4_back.map(lambda (dLdl3, dLdA4, dLdb4) : dLdb4).reduce(lambda a,b : a+b)

    """ TODO: Layer3: Pool (16 x 16 x 16) Backward """ 
    dLdl2 = dLdl3.zip(layer2).zip(X_idx3).map(lambda ((dLdl3, layer2), X_idx3) :  max_pool_backward(dLdl3, layer2, X_idx3, F3, S3))
   
    """ TODO: Layer2: ReLU (32 x 32 x 16) Backward """
    dLdl1 = dLdl2.zip(layer1).map(lambda (dLdl2, layer1) : ReLU_backward(dLdl2, layer1))
     
    """ TODO: Layer1: Conv (32 x 32 x 16) Backward """ 
    l1_back = dLdl1.zip(X).zip(X_col1).map(lambda ((dLdl1, X), X_col1) :  conv_backward(dLdl1, X, X_col1, A1, S1, P1))
    dLdX = l1_back.map(lambda (dLdX, dLdA1, dLdb1) : dLdX)
    dLdA1 = l1_back.map(lambda (dLdX, dLdA1, dLdb1) : dLdA1).reduce(lambda a,b : a+b)
    dLdb1 = l1_back.map(lambda (dLdX, dLdA1, dLdb1) : dLdb1).reduce(lambda a,b : a+b)

    """ TODO: reduce gradients """

    """ gradient scaling """
    L /= float(count)
    dLdA1 /= float(count)
    dLdb1 /= float(count)
    dLdA4 /= float(count)
    dLdb4 /= float(count)
    dLdA7 /= float(count)
    dLdb7 /= float(count)
    dLdA10 /= float(count)
    dLdb10 /= float(count)

    """ regularization """
    L += 0.5 * self.lam * np.sum(self.A1*self.A1)
    L += 0.5 * self.lam * np.sum(self.A4*self.A4)
    L += 0.5 * self.lam * np.sum(self.A7*self.A7)
    L += 0.5 * self.lam * np.sum(self.A10*self.A10)

    """ regularization gradient """
    dLdA10 = dLdA10.reshape(self.A10.shape)
    dLdA7 = dLdA7.reshape(self.A7.shape)
    dLdA4 = dLdA4.reshape(self.A4.shape)
    dLdA1 = dLdA1.reshape(self.A1.shape)
    dLdA10 += self.lam * self.A10
    dLdA7 += self.lam * self.A7
    dLdA4 += self.lam * self.A4
    dLdA1 += self.lam * self.A1

    """ tune the parameter """
    self.v1 = self.mu * self.v1 - self.rho * dLdA1
    self.v4 = self.mu * self.v4 - self.rho * dLdA4
    self.v7 = self.mu * self.v7 - self.rho * dLdA7
    self.v10 = self.mu * self.v10 - self.rho * dLdA10
    self.A1 += self.v1
    self.A4 += self.v4 
    self.A7 += self.v7
    self.A10 += self.v10
    self.b1 += -self.rho * dLdb1
    self.b4 += -self.rho * dLdb4
    self.b7 += -self.rho * dLdb7
    self.b10 += -self.rho * dLdb10

    return L

