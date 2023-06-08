using Hanabi;

namespace Sandbox
{
    public class Player : Entity
    {
        public float Speed;
        public float Time = 0.0f;

        private Rigidbody2DComponent m_Rigidbody;
        private SpriteRendererComponent m_SpriteRenderer;
        private Vector2[] SpriteTextureCoords = new Vector2[4];

        void OnCreate()
        {
            m_Rigidbody = GetComponent<Rigidbody2DComponent>();
            m_SpriteRenderer = GetComponent<SpriteRendererComponent>();

            SpriteTextureCoords[0] = new Vector2(0.0f, 0.5f); //Right
            SpriteTextureCoords[1] = new Vector2(0.1f, 1.0f);

            SpriteTextureCoords[2] = new Vector2(0.0f, 0.0f); //Left
            SpriteTextureCoords[3] = new Vector2(0.1f, 0.5f);

            if (m_SpriteRenderer == null)
                Log.Error("SpriteRendererComponent is null");
        }

        void OnUpdate(float ts)
        {
            Time += ts;

            Vector3 velocity = Vector3.Zero;

            if (Input.IsKeyDown(KeyCode.A))
            {
                velocity.X = -1.0f;
                m_SpriteRenderer.SetTextureCoords(SpriteTextureCoords[2], SpriteTextureCoords[3]);
            }
            else if (Input.IsKeyDown(KeyCode.D))
            {
                velocity.X = 1.0f;
                m_SpriteRenderer.SetTextureCoords(SpriteTextureCoords[0], SpriteTextureCoords[1]);
            }

            velocity *= Speed * ts;

            m_Rigidbody.ApplyLinearImpulse(velocity.XY, true);
        }
    }
}