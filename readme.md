ez80_parse

This tool predicts the state of registers on the eZ80. This can be useful for finding optimizations and etc.

# register prediction tool

The register prediction tool will keep track of register values and flags. It also supports CRT function and LIBC functions to improve predictions.

The bits of a register/flag can be in one of three states: known zero/false, known one/true, or unknown. A 4 bit group of bits will display a hexadecimal digit `0123456789ABCDEF` if it is fully known, `*` if there is a mix of known and unknown bits, and `-` if no bits are known. Flags will display `?` if they are unknown, `-` if they are known to be false, and `SZVC` if they are known to be true.

Each row will display the state of the regsiters/flags after the instruction has been executed. All conditional jumps (JR/JQ/JP) are assumed to be not taken.

By default the following registers are shown: (SZVC = sign, zero, overflow, carry)
`(SP + 3) | (SP + 0) | IX | IY | A | BC | DE | HL | SZVC | instruction`

Example register prediction output:
```
487: A -- | BC 00FFFF | DE ------ | HL ------ | ???? | ld    bc, $00ffff
488: A -- | BC 00FFFF | DE ------ | HL ------ | ???? | push  de
489: A -- | BC 00FFFF | DE ------ | HL ------ | ???? | pop   hl
490: A -- | BC 00FFFF | DE ------ | HL 00---- | ???? | call  __iand
491: A -- | BC 00FFFF | DE ------ | HL 0*---- | ???- | add   hl, bc
492: A -- | BC 00FFFF | DE ------ | HL 0*---- | ???- | or    a, a
493: A -- | BC 00FFFF | DE ------ | HL ------ | ??-? | sbc   hl, bc
494: A -- | BC 00FFFF | DE ------ | HL 000000 | -Z-- | jr    nz, BB1_2
495: A 00 | BC 00FFFF | DE ------ | HL 000000 | -Z-- | ld    a, $00
```

In the example above. `HL = HL & 0x00FFFF` is performed. So we know that the upper 8 bits of `HL` are zero. We then calculate `HL = 0x00???? + 0x00FFFF`. We are not sure what the result will be, but we do know that the result won't touch bits [17, 23], although the result may overflow to bit 16 which is why it is now set to unknown. Carry will be cleared since we know the result won't overflow to bit 24. The result of `sbc hl, bc` won't be known, however we can conclude that a signed overflow cannot occur since we know that `HL` and `BC` are both positive. This means that the overflow flag will be false. After performing `sbc hl, bc` we encounter a jump if non-zero. If the jump is not taken, then we can conclude that `HL` must be equal to zero.

# supported assembler extensions

- `jq` will be treated as `jp`
