package br.brazucas.editor;

import com.android.apksig.ApkSigner;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.security.KeyFactory;
import java.security.PrivateKey;
import java.security.cert.CertificateFactory;
import java.security.cert.X509Certificate;
import java.security.spec.PKCS8EncodedKeySpec;
import java.util.Collections;

/**
 * Reassina um APK com a chave da equipe Brazucas Pet usando assinatura
 * v1 + v2 + v3 (necessaria para instalar em Android 11+ quando o jogo
 * tem targetSdk 30 ou maior).
 *
 * Chamado pelo APK Editor logo depois que ele gera o APK editado.
 */
public final class Assinador {

    private Assinador() {}

    /**
     * @param chavePk8 chave privada PKCS#8 (DER)
     * @param certPem  certificado X.509 (PEM ou DER)
     * @param caminho  APK que acabou de ser gerado; e substituido pelo APK reassinado
     */
    public static void reassinar(InputStream chavePk8, InputStream certPem, String caminho) throws Exception {
        byte[] chave = lerTudo(chavePk8);
        PrivateKey privada = carregarChave(chave);
        X509Certificate cert = (X509Certificate) CertificateFactory.getInstance("X.509")
                .generateCertificate(certPem);

        File entrada = new File(caminho);
        File temp = new File(caminho + ".brz.tmp");
        if (temp.exists() && !temp.delete()) {
            throw new IOException("Nao foi possivel apagar " + temp);
        }

        ApkSigner.SignerConfig signer = new ApkSigner.SignerConfig.Builder(
                "BRAZUCAS", privada, Collections.singletonList(cert)).build();

        ApkSigner apkSigner = new ApkSigner.Builder(Collections.singletonList(signer))
                .setInputApk(entrada)
                .setOutputApk(temp)
                .setV1SigningEnabled(true)
                .setV2SigningEnabled(true)
                .setV3SigningEnabled(true)
                .setV4SigningEnabled(false)
                .setCreatedBy("Brazucas Editor")
                .build();
        try {
            apkSigner.sign();
        } catch (Exception e) {
            temp.delete();
            throw new Exception("Falha ao reassinar com a chave Brazucas: " + e, e);
        }

        if (!entrada.delete() || !temp.renameTo(entrada)) {
            throw new IOException("APK reassinado ficou em " + temp + " (nao consegui substituir o original)");
        }
    }

    private static PrivateKey carregarChave(byte[] pk8) throws Exception {
        PKCS8EncodedKeySpec spec = new PKCS8EncodedKeySpec(pk8);
        for (String alg : new String[] {"RSA", "EC", "DSA"}) {
            try {
                return KeyFactory.getInstance(alg).generatePrivate(spec);
            } catch (Exception ignored) {
                // tenta o proximo algoritmo
            }
        }
        throw new Exception("Chave .pk8 invalida");
    }

    private static byte[] lerTudo(InputStream in) throws IOException {
        try {
            ByteArrayOutputStream out = new ByteArrayOutputStream();
            byte[] buf = new byte[8192];
            int n;
            while ((n = in.read(buf)) > 0) {
                out.write(buf, 0, n);
            }
            return out.toByteArray();
        } finally {
            in.close();
        }
    }

    /** Uso no PC: java -cp ... br.brazucas.editor.Assinador chave.pk8 cert.x509.pem app.apk */
    public static void main(String[] args) throws Exception {
        reassinar(new java.io.FileInputStream(args[0]), new java.io.FileInputStream(args[1]), args[2]);
        System.out.println("OK: " + args[2]);
    }
}
