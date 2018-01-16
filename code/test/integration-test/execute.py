import os
import subprocess
import time
import filecmp
import sys


def update_progress(progress):
    barLength = 20 # Modify this to change the length of the progress bar
    status = ""
    if isinstance(progress, int):
        progress = float(progress)
    if not isinstance(progress, float):
        progress = 0
        status = "error: progress var must be float\r\n"
    if progress < 0:
        progress = 0
        status = "Halt...\r\n"
    if progress >= 1:
        progress = 1
        status = "Done...\r\n"
    block = int(round(barLength*progress))
    text = "\rExecuting network tests: [{0}] {1}% {2}".format( "#"*block + "-"*(barLength-block), progress*100, status)
    sys.stderr.write(text)
    sys.stderr.flush()

if __name__ == '__main__':
    os.chdir('../../build')
    dump = open('../test/integration-test/io/dump.txt', 'w')

    sender_1_params = ['./nachos-step6', '-x', './network_sender', '-l', '0.75', '-m', '0', '-rs', '18']
    receiver_1_params = ['./nachos-step6', '-x', './network_receiver', '-l', '0.75', '-m', '1', '-rs', '18']

    sender_2_params = ['./nachos-step6', '-x', './network_test2_snd', '-l', '1.0', '-m', '1', '-rs', '18']
    receiver_2_params = ['./nachos-step6', '-x', './network_test2_rcv', '-l', '1.0', '-m', '2', '-rs', '18']

    num_tests = 10
    total_tests = num_tests * 2

    update_progress(0)
    for i in xrange(num_tests):
        try:
            os.remove('./OMG_THIS_IS_HALT.O')
        except:
            pass


        sender_1 = subprocess.Popen(sender_1_params, stdout=dump)
        time.sleep(3)
        receiver_1 = subprocess.Popen(receiver_1_params, stdout=dump)

        sender_1.wait()
        code = receiver_1.wait()

        status = filecmp.cmp('./OMG_THIS_IS_HALT.O', './step6-usersemaphore.o')
        update_progress(float(i + 1) / float(total_tests))
        if not status:
            print '>>>> FAILED 1'
            if code == 0:
                exit(1)

    for i in xrange(num_tests):
        sender_2 = subprocess.Popen(sender_2_params, stdout=dump)
        time.sleep(3)
        receiver_2 = subprocess.Popen(receiver_2_params, stdout=dump)

        ret_code_sender = sender_2.wait()
        ret_code_receiver = receiver_2.wait()

        update_progress(float(i + num_tests + 1) / float(total_tests))

        if (ret_code_sender != 0) or (ret_code_receiver != 0):
            print '>>>> FAILED 2'
            exit(2)

    print 'SUCCESS'


