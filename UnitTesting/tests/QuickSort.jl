a::Array{Int} = [7,4,9,2,10,1,5,3,8,6,false]

function quickSort(left::Int, right::Int)::Int
    i::Int = left
    j::Int = right
    tmp::Int =0
    pivot::Int = a[(left + right) / 2]

    while (i <= j)
        while (a[i] < pivot)
            i = i+1
        end

        while (a[j] > pivot)
            j = j-1
        end
        if (i <= j)
            tmp = a[i]
            a[i] = a[j]
            a[j] = tmp
            i = i+1
            j = j-1
        end
    end

    if (left < j)
        quickSort(left, j)
    end
    if (i < right)
        quickSort(i, right)
    end
end


for i = 1:11
    if(i != 11)
        print(a[i], ", ")
    else
        println(a[i])
    end
end

println("---------------Ya Ordenado----------------------")

quickSort(1, 11)

for i = 1:11
    if(i != 11)
        print(a[i], ", ")
    else
        println(a[i])
    end
end