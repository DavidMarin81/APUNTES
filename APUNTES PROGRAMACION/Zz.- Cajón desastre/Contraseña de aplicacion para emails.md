# Contraseña de aplicación
## ¿Qué es?
- Una contraseña de aplicación es una contraseña especial que Google genera para usar en apps externas, siempre que se tenga la verificación en dos pasos activada (2FA)
- No es tu contraseña normal de Gmail
- Solo funciona en la app o servicio qeu la uses
- Google la muestra solo una vez por seguridad

## Ejemplo
- Si quieres enviar un correo desde tu aplicación C# usando Gmail:
    - Activa 2FA en tu cuenta Gmail
    - Genera una contraseña de aplicación
    - En tu código SMTP pones:
        ~~~csharp
        await client.AuthenticateAsync("tucuenta@gmail.com", "xgfrlzsatqwemnop");
        ~~~
    - Gmail reconoce que esa contraseña solo sirve para esta app
    - No necesitas poner el código de 2FA cada vez
    - Si pierdes la contraseña o crees que alguien la tiene, la puedes revocar sin afectar tu contraseña normal

## Google
- Para obtenerla:
    - Hay que tener activa la verificación en dos pasos en la cuenta Google
    - Ir aqui 👉 https://myaccount.google.com/apppasswords 👈
    - En "Contraseñas de aplicación" crear una
    - Si ya se creó una, Google no te deja volver a verla