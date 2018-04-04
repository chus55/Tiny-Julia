x::Int = 10

function print_til_zero()::Int
    print(x)
    x = x-1
    if(x != 0)
        print(", ")
    end
    if(x > 0)
        print_til_zero()
    else
        println("")
    end
end

print_til_zero()
