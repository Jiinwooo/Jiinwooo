"""Neutraliza a checagem de assinatura do libsyscheck.so do APK Editor 1.9.0.
A funcao de verificacao passa a devolver (arg ^ 0x55555555), que e o valor de 'assinatura original'.
Isso desliga: (1) o abort() 5s depois, (2) a corrupcao do nome da chave no Java."""
import sys, struct, hashlib
P = {
 'arm64-v8a':   (0x19f4c, bytes.fromhex('a0aa8a52a0aaaa72400000 4ac0035fd6'.replace(' ',''))),
 'armeabi-v7a': (0x1f190, struct.pack('<4I',0xe3050555,0xe3450555,0xe0220000,0xe12fff1e)),
 'armeabi':     (0x1fe2c, struct.pack('<5I',0xe3a00055,0xe1800400,0xe1800800,0xe0220000,0xe12fff1e)),
 'x86':         (0x1a643, bytes.fromhex('8b44240c3555555555c3')),
}
EXPECT = {  # prologo original esperado
 'arm64-v8a': 'fd7bb0a9', 'armeabi-v7a': None, 'armeabi': None, 'x86': None,
}
root = sys.argv[1]
for abi,(off,code) in P.items():
    p=f'{root}/lib/{abi}/libsyscheck.so'; d=bytearray(open(p,'rb').read())
    print(abi, 'antes', d[off:off+len(code)].hex(), '->', code.hex())
    d[off:off+len(code)] = code
    open(p,'wb').write(d)
