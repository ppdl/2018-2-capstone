import tensorflow as tf
from tensorflow.contrib.layers import fully_connected, batch_norm, dropout
from tensorflow.contrib.framework import arg_scope
import numpy as np
import argparse
import socket
from time import sleep

TERMINAL0 = '10.42.0.223'
TERMINAL1 = '10.42.0.217'
TERMINAL2 = '10.42.0.190'
PORT = 56789

keep_prob = 0.7
X = tf.placeholder(tf.float32, [None, 9])
Y = tf.placeholder(tf.float32, [None, 3])
train_mode = tf.placeholder(tf.bool, name='train_mode')

# layer output size
# hidden_output_size = 512
hidden_output_size = 40
final_output_size = 3

xavier_init = tf.contrib.layers.xavier_initializer()

bn_params = {
    'is_training': train_mode,
    'decay': 0.9,
    'updates_collections': None
}

# weights_regularizer = tf.contrib.layers.l2_regularizer(cfg.TRAIN.WEIGHT_DECAY)
# weights_regularizer = tf.contrib..l2_regularizer(cfg.TRAIN.WEIGHT_DECAY)

# We can build short code using 'arg_scope' to avoid duplicate code
# same function with different arguments
with arg_scope([fully_connected],
               activation_fn=tf.nn.relu,
               weights_regularizer=tf.contrib.layers.l2_regularizer(scale=0.05),
               weights_initializer=xavier_init,
               # weights_regularizer=fully_connected.l2_regularizer(0.05),
               biases_initializer=None,
               normalizer_fn=batch_norm,
               normalizer_params=bn_params
               ):
    hidden_layer1 = fully_connected(X, hidden_output_size, scope="h1")
    h1_drop = dropout(hidden_layer1, keep_prob, is_training=train_mode)
    hidden_layer2 = fully_connected(h1_drop, hidden_output_size, scope="h2")
    h2_drop = dropout(hidden_layer2, keep_prob, is_training=train_mode)
    hidden_layer3 = fully_connected(h2_drop, hidden_output_size, scope="h3")
    h3_drop = dropout(hidden_layer3, keep_prob, is_training=train_mode)
    hidden_layer4 = fully_connected(h3_drop, hidden_output_size, scope="h4")
    h4_drop = dropout(hidden_layer4, keep_prob, is_training=train_mode)
    hypothesis = fully_connected(h4_drop, final_output_size, activation_fn=None, scope="hypothesis")

# define cost/loss & optimizer
cost = tf.reduce_mean(tf.square(np.array(hypothesis) - np.array(Y)))

# optimizer = tf.train.AdamOptimizer(learning_rate=learning_rate).minimize(cost)
optimizer = tf.train.GradientDescentOptimizer(learning_rate=0.1).minimize(cost)


def mkdelay (collision,delayrate):
    limitrate = 0.1
    d1 = collision[0][0]
    d2 = collision[0][1]
    d3 = collision[0][2]
    if d1 < limitrate: d1 = 0.0
    if d2 < limitrate: d2 = 0.0
    if d3 < limitrate: d3 = 0.0
    d1 = d1 * delayrate
    d2 = d2 * delayrate
    d3 = d3 * delayrate
    return (d1, d2, d3)

def main():
    parser = argparse.ArgumentParser(description='Option')
    parser.add_argument('Read_TBI', type= str, default='TBI.txt', help="Read TBI file name")
    parser.add_argument('Write_Delay', type=str, default='Delay.txt', help="Write delay file name")
    args = parser.parse_args()
    f = open(args.Read_TBI, 'r')
    #ff = open(args.Write_Delay, 'w')
    saver = tf.train.Saver()
    sess = tf.Session()
    sess.run(tf.global_variables_initializer())
    tmp = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    t = None
    start = False
    sum1=0
    sum2=0
    sum3=0
    count=0
    try:
        while True :
            line = f.readline()
            f.seek(0)
            num = line.split(',')
            num = list(map(float, num))
            t = num
            num = list(np.array(num) - np.array(tmp))
            tmp = t
            feed_dict_ex = {X: np.array([num]), train_mode: False}
            saver.restore(sess,'./Collisionlearning-nondiv.ckpt')
            collision = sess.run(hypothesis,feed_dict=feed_dict_ex)
            delayrate = 1
            (d1, d2, d3) = mkdelay(collision,delayrate)
            '''
            count += 1
            sum1 += d1
            sum2 += d2
            sum3 += d3
            if d1:
                d1 -= 1.1
            if d2:
                d2 -= 0
            if d3:
                d3 -= 0.1
            if not start:
                start = True
                continue
            '''
            d1*=1000
            d2*=1000
            d3*=1000
            #print(int(d1*1000), int(d2*1000), int(d3*1000))
            for TERMINAL,DELAY in zip([TERMINAL0, TERMINAL1, TERMINAL2],[int(d1),int(d2),int(d3)]):
                with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
                    print('trying to connect terminal ' + TERMINAL + ' delay: ' + str(DELAY))
                    s.connect((TERMINAL,PORT))
                    s.sendall(str(DELAY).encode())
            '''
            with open(args.Write_Delay, 'w') as ff:
                ff.write(str(d1)+","+str(d2)+","+str(d3))
            print(str(d1)+","+str(d2)+","+str(d3))
            '''
            sleep(1)

    except KeyboardInterrupt : # Ctrl + C
        ff.close()
        f.close()
        exit(1)

    ff.close()
    f.close()
    #chkp.print_tensors_in_checkpoint_file("Collisionlearning.ckpt", tensor_name='h3/weights', all_tensors=False)


if __name__ == '__main__':
    main()
