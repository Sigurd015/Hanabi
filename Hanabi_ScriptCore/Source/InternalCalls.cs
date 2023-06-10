using System;
using System.Runtime.CompilerServices;

namespace Hanabi
{
    public static class InternalCalls
    {
        #region Entity
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static object GetScriptInstance(ulong entityID);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Entity_HasComponent(ulong entityID, Type componentType);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static ulong Entity_FindEntityByName(string name);
        #endregion

        #region TransformComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_GetTranslation(ulong entityID, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TransformComponent_SetTranslation(ulong entityID, ref Vector3 translation);
        #endregion

        #region Rigidbody2DComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulse(ulong entityID, ref Vector2 impulse, ref Vector2 point, bool wake);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(ulong entityID, ref Vector2 impulse, bool wake);
        #endregion

        #region SpriteRendererComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void SpriteRendererComponent_SetTextureCoords(ulong entityID, ref Vector2 start, ref Vector2 end);
        #endregion

        #region TextComponent
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static string TextComponent_GetText(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TextComponent_SetText(ulong entityID, string text);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TextComponent_GetColor(ulong entityID, out Vector4 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TextComponent_SetColor(ulong entityID, ref Vector4 color);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float TextComponent_GetKerning(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TextComponent_SetKerning(ulong entityID, float kerning);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static float TextComponent_GetLineSpacing(ulong entityID);
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void TextComponent_SetLineSpacing(ulong entityID, float lineSpacing);
        #endregion

        #region Input
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyDown(KeyCode keycode);
        #endregion

        #region Log
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Log_LogMessage(LogLevel level, string message);
        #endregion
    }
}