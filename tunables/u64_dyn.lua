function pack_u64_dyn(v)
    local out = {}
    for i = 1, 8 do
        local cur = v & 0x7f
        v = v >> 7
        if v ~= 0 then
            out[#out + 1] = string.char(cur | 0x80)
        else
            out[#out + 1] = string.char(cur)
            return table.concat(out)
        end
    end
    if v ~= 0 then
        out[#out + 1] = string.char(v)
    end
    return table.concat(out)
end

function unpack_u64_dyn(str)
    local v = 0
    local i = 1
    local bits = 0
    while true do
        local b = str:byte(i)
        i = i + 1
        local has_next = (bits < 56) and (b >> 7) ~= 0
        if has_next then
            b = b & 0x7f
        end
        v = v | (b << bits)
        if not has_next then
            break
        end
        bits = bits + 7
    end
    return v
end

function pack_i64_dyn(v)
    local neg = (v >> 63)
    if v < 0 then
        v = (v * -1) & ~(1 << 63)
    end
    return pack_u64_dyn((neg << 6) | ((v & ~0x3f) << 1) | (v & 0x3f))
end

function unpack_i64_dyn(str)
    local v = unpack_u64_dyn(str)
    local neg = ((v >> 6) & 1) ~= 0
    v = ((v >> 1) & ~0x3f) | (v & 0x3f)
    if neg then
        if v == 0 then
            v = 1 << 63
        else
            v = v * -1
        end
    end
    return v
end
