test1::Int = 1
while(test1 <= 10)   
    println(test1)
    if test1 == 7
        println("Aqui paro en 7")
        break
    end
    test1 = test1 + 1
end
                        #=Prints
1
23
4
5
6
7
Aqui paro en 7
                        =#