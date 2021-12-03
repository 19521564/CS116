import shutil
from sys import argv
from ortools.algorithms import pywrapknapsack_solver
import os
import glob
from shutil import copyfile
import random
import pandas as pd
import time

def chooseTests(source,destination):
    desFolders = glob.glob(destination + '/*')
    for desFolder in desFolders:
        for f in glob.glob(desFolder + '/*.solution'):
            os.remove(f)

    folders = glob.glob(source + '/*')
    numberFolders = ['n00050','n00100','n00200','n00500','n01000']
    nextFolders = ['R01000','R10000']
    for p in folders:
        if os.path.isdir(p):
            p = p.replace("\\", "/")
            testNumbers = [i for i in range(100)]
            for nFolder in numberFolders:
                folder_name = p.split('/')[-1]
                nextFolder = random.choice(nextFolders)
                testNumber = random.choice(testNumbers)
                testNumbers.remove(testNumber)
                testName = ''
                if testNumber > 9:
                    testName = 's0' + str(testNumber) + '.kp'
                else:
                    testName = 's00' + str(testNumber) + '.kp'

                
                
                s = '{}/{}/{}/{}/{}'.format(source,folder_name,nFolder,nextFolder,testName)
                d = '{}/{}/{}'.format(destination,folder_name,testName)
                copyfile(s,d)
def getTestPaths(path):
    testpaths = []
    folders = glob.glob(path + '/*')
    for folder in folders:
        tests = glob.glob(folder + '/*.kp')
        testpaths += tests
    
    return testpaths

def getTestData(path):
    f = open(path,'r')
    arr = f.read().split('\n')[:-1]
    f.close()

    n = int(arr[1])
    capacity = int(arr[2])
    n_count = 0
    values = []
    weights = []
    for pair in arr[4:]:
        n_count+=1
        pair = pair.split()
        values.append(int(pair[0]))
        weights.append(int(pair[1]))

    if n_count != n:
        raise Exception('n_count != n: {} != {}'.format(n_count,n))

    return n,[capacity], values, [weights]
def main():

    testPaths = getTestPaths('C:/CS106/knapsack/kplib-master')
    timeLimit = 60
    table = []

    for testPath in testPaths:
        testPath = testPath.replace("\\","/")
        # Create the solver.
        solver = pywrapknapsack_solver.KnapsackSolver(
        pywrapknapsack_solver.KnapsackSolver.
        KNAPSACK_MULTIDIMENSION_BRANCH_AND_BOUND_SOLVER, 'KnapsackExample')
        solver.set_time_limit(timeLimit)

        print("Solving '{}'...".format(testPath))
        groupName = testPath.split('/')[-2]
        testName = testPath.split('/')[-1]
        n,capacities, values, weights = getTestData(testPath)
        solver.Init(values, weights, capacities)
        
        t0 = time.time()
        computedValue = solver.Solve()
        t = time.time() - t0

        packedItems = []
        packedWeights = []
        packedValues = []
        totalWeight = 0
        for i in range(len(values)):
            if solver.BestSolutionContains(i):
                packedItems.append(i)
                packedValues.append(values[i])
                packedWeights.append(weights[0][i])
                totalWeight += weights[0][i]
        
        del solver

        if sum(packedWeights) != totalWeight or sum(packedValues) != computedValue:
            print(sum(packedWeights), totalWeight)
            print(sum(packedValues) , computedValue)
            raise Exception('Solution Error!')
        s = '\n'.join([str(n),str(computedValue),str(totalWeight),' '.join(list(map(str,packedItems))),' '.join(list(map(str,packedValues))),' '.join(list(map(str,packedWeights)))])

        resultPath = os.path.join(os.path.split(testPath)[0],testName.split('.')[0] + '.solution')
        f = open(resultPath,'w')
        f.write(s)
        f.close()
        
        exp = 0.2 #trường hợp nhỏ hơn nhưng sấp xỉ timeLimit, ví dụ 29.99 < 30 
        isOptional = 1 if t<(timeLimit-exp) else 0
        result = [groupName,testName,n,computedValue,totalWeight,t, isOptional]
        print(result)
        table.append(result)

    table = pd.DataFrame(table,index=None,columns=['test_case_group','name_ testcase','number_knapsack' ,'total_value', 'total_weight', 'time_run', 'is_solution_optional?'])
    print(table)
    table.to_csv('KetQua.csv')
if __name__ == '__main__':
    # chooseTests('C:/Users/DELL/Downloads/kplib-master/kplib-master','C:/CS106/knapsack/kplib-master')
    main()
