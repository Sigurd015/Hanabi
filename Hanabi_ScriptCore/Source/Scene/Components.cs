namespace Hanabi
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class TransformComponent : Component
    {
        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(Entity.ID, out Vector3 translation);
                return translation;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(Entity.ID, ref value);
            }
        }
    }

    public class CameraComponent : Component
    { }

    public class SpriteRendererComponent : Component
    {
        public void SetTextureCoords(Vector2 start, Vector2 end)
        {
            InternalCalls.SpriteRendererComponent_SetTextureCoords(Entity.ID, ref start, ref end);
        }
    }

    public class CircleRendererComponent : Component
    { }


    public class ScriptComponent : Component
    { }

    public class Rigidbody2DComponent : Component
    {
        public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPosition, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyLinearImpulse(Entity.ID, ref impulse, ref worldPosition, wake);
        }

        public void ApplyLinearImpulse(Vector2 impulse, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyLinearImpulseToCenter(Entity.ID, ref impulse, wake);
        }
    }

    public class BoxCollider2DComponent : Component
    { }

    public class CircleCollider2DComponent : Component
    { }
}