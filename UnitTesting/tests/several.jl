el_global::Int = 100
el_bool::Bool = false
function IsPair(val::Int)::Bool
    return val % 2 == 0
end

function WeirdStuff(val::Int)::Int
    valor::Int = val * 2;

    for i = 0:valor
        if i == 5
            println("The favorite number, ", i, ", has been reached: ", valor);
        end
        println("Is pair ", i, " the val: ", IsPair(i))
    end

    return valor * el_global
end

println("#", el_global, " is pair: ", IsPair(el_global))
println("Weird stuff from val: ", el_global, " now: ", WeirdStuff(el_global))

if el_bool == false
    println("All a lie.")
end

#=#=
#100 is pair: true
Is pair 0 the val: true
Is pair 1 the val: false
Is pair 2 the val: true
Is pair 3 the val: false
Is pair 4 the val: true
The favorite number, 5, has been reached: 200
Is pair 5 the val: falseIs pair 6 the val: true
Is pair 7 the val: false
Is pair 8 the val: true
Is pair 9 the val: false
Is pair 10 the val: true
Is pair 11 the val: false
Is pair 12 the val: true
Is pair 13 the val: false
Is pair 14 the val: true
Is pair 15 the val: false
Is pair 16 the val: true
Is pair 17 the val: false
Is pair 18 the val: true
Is pair 19 the val: false
Is pair 20 the val: true
Is pair 21 the val: false
Is pair 22 the val: true
Is pair 23 the val: false
Is pair 24 the val: true
Is pair 25 the val: false
Is pair 26 the val: true
Is pair 27 the val: false
Is pair 28 the val: true
Is pair 29 the val: false
Is pair 30 the val: true
Is pair 31 the val: false
Is pair 32 the val: true
Is pair 33 the val: false
Is pair 34 the val: true
Is pair 35 the val: false
Is pair 36 the val: true
Is pair 37 the val: false
Is pair 38 the val: true
Is pair 39 the val: false
Is pair 40 the val: true
Is pair 41 the val: false
Is pair 42 the val: true
Is pair 43 the val: false
Is pair 44 the val: true
Is pair 45 the val: false
Is pair 46 the val: true
Is pair 47 the val: false
Is pair 48 the val: true
Is pair 49 the val: false
Is pair 50 the val: true
Is pair 51 the val: false
Is pair 52 the val: true
Is pair 53 the val: false
Is pair 54 the val: true
Is pair 55 the val: false
Is pair 56 the val: true
Is pair 57 the val: false
Is pair 58 the val: true
Is pair 59 the val: false
Is pair 60 the val: true
Is pair 61 the val: false
Is pair 62 the val: true
Is pair 63 the val: false
Is pair 64 the val: true
Is pair 65 the val: false
Is pair 66 the val: true
Is pair 67 the val: false
Is pair 68 the val: true
Is pair 69 the val: false
Is pair 70 the val: true
Is pair 71 the val: false
Is pair 72 the val: true
Is pair 73 the val: false
Is pair 74 the val: true
Is pair 75 the val: false
Is pair 76 the val: true
Is pair 77 the val: false
Is pair 78 the val: true
Is pair 79 the val: false
Is pair 80 the val: true
Is pair 81 the val: false
Is pair 82 the val: true
Is pair 83 the val: false
Is pair 84 the val: true
Is pair 85 the val: false
Is pair 86 the val: true
Is pair 87 the val: false
Is pair 88 the val: true
Is pair 89 the val: false
Is pair 90 the val: true
Is pair 91 the val: false
Is pair 92 the val: true
Is pair 93 the val: false
Is pair 94 the val: true
Is pair 95 the val: false
Is pair 96 the val: true
Is pair 97 the val: false
Is pair 98 the val: true
Is pair 99 the val: false
Is pair 100 the val: true
Is pair 101 the val: false
Is pair 102 the val: true
Is pair 103 the val: false
Is pair 104 the val: true
Is pair 105 the val: false
Is pair 106 the val: true
Is pair 107 the val: false
Is pair 108 the val: true
Is pair 109 the val: false
Is pair 110 the val: true
Is pair 111 the val: false
Is pair 112 the val: true
Is pair 113 the val: false
Is pair 114 the val: true
Is pair 115 the val: false
Is pair 116 the val: true
Is pair 117 the val: false
Is pair 118 the val: true
Is pair 119 the val: false
Is pair 120 the val: true
Is pair 121 the val: false
Is pair 122 the val: true
Is pair 123 the val: false
Is pair 124 the val: true
Is pair 125 the val: false
Is pair 126 the val: true
Is pair 127 the val: false
Is pair 128 the val: true
Is pair 129 the val: false
Is pair 130 the val: true
Is pair 131 the val: false
Is pair 132 the val: true
Is pair 133 the val: false
Is pair 134 the val: true
Is pair 135 the val: false
Is pair 136 the val: true
Is pair 137 the val: false
Is pair 138 the val: true
Is pair 139 the val: false
Is pair 140 the val: true
Is pair 141 the val: false
Is pair 142 the val: true
Is pair 143 the val: false
Is pair 144 the val: true
Is pair 145 the val: false
Is pair 146 the val: true
Is pair 147 the val: false
Is pair 148 the val: true
Is pair 149 the val: false
Is pair 150 the val: true
Is pair 151 the val: false
Is pair 152 the val: true
Is pair 153 the val: false
Is pair 154 the val: true
Is pair 155 the val: false
Is pair 156 the val: true
Is pair 157 the val: false
Is pair 158 the val: true
Is pair 159 the val: false
Is pair 160 the val: true
Is pair 161 the val: false
Is pair 162 the val: true
Is pair 163 the val: false
Is pair 164 the val: true
Is pair 165 the val: false
Is pair 166 the val: true
Is pair 167 the val: false
Is pair 168 the val: true
Is pair 169 the val: false
Is pair 170 the val: true
Is pair 171 the val: false
Is pair 172 the val: true
Is pair 173 the val: false
Is pair 174 the val: true
Is pair 175 the val: false
Is pair 176 the val: true
Is pair 177 the val: false
Is pair 178 the val: true
Is pair 179 the val: false
Is pair 180 the val: true
Is pair 181 the val: false
Is pair 182 the val: true
Is pair 183 the val: false
Is pair 184 the val: true
Is pair 185 the val: false
Is pair 186 the val: true
Is pair 187 the val: false
Is pair 188 the val: true
Is pair 189 the val: false
Is pair 190 the val: true
Is pair 191 the val: false
Is pair 192 the val: true
Is pair 193 the val: false
Is pair 194 the val: true
Is pair 195 the val: false
Is pair 196 the val: true
Is pair 197 the val: false
Is pair 198 the val: true
Is pair 199 the val: false
Is pair 200 the val: true
Weird stuff from val: 100 now: 20000
All a lie.

#=#