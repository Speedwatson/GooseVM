import random
arr = list(range(int(input())))
random.shuffle(arr)

n = len(arr)
b = [0 for i in range(n)]

def mid(start, end):
    return start + (end - start) // 2


def merge_sort(start, end):
    if start == end: 
        return
        
    print("merge_sorting", repr(arr[start:end + 1]))
        

    merge_sort(start, mid(start, end))
    merge_sort(mid(start, end) + 1, end)

    merge(start, end)

def merge(start, end):
    print("merging", repr(arr[start : start + (end - start) // 2 + 1]), "and", repr(arr[start + (end - start) // 2 + 1 : end + 1]))
    k = 0
    i = start
    j = mid(start, end) + 1
    while k < (end - start) + 1:
        if i == mid(start, end) + 1:
            b[k] = arr[j]   # sel.second
            j += 1
        elif j == end + 1:
            b[k] = arr[i]   # sel.first
            i += 1
        elif arr[i] < arr[j]:
            b[k] = arr[i]   # sel.first
            i += 1
        else:
            b[k] = arr[j]   # sel.second
            j += 1

        k += 1

    k = 0
    while k < (end - start) + 1:
        arr[start + k] = b[k]
        k += 1

    print("Merged:", repr(arr[start : end + 1]))


merge_sort(0, n - 1)
print(repr(arr))
