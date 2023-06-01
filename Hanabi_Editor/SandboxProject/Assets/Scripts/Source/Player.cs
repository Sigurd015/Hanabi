using Hanabi;

namespace Sandbox
{
    public class Player : Entity
    {
        private TransformComponent m_Transform;
        private Rigidbody2DComponent m_Rigidbody;

        public float Speed;
        public float Time = 0.0f;

        void OnCreate()
        {
            Log.Info($"Player.OnCreate - {ID}");
            m_Transform = GetComponent<TransformComponent>();
            m_Rigidbody = GetComponent<Rigidbody2DComponent>();
        }

        void OnUpdate(float ts)
        {
            Time += ts;

            Log.Info($"Player.OnUpdate - {m_Transform.Translation}");
            Log.Warn($"Player.OnUpdate - {Time}");
            Log.Trace($"Player.OnUpdate - {Time}");
            Log.Critical($"Player.OnUpdate - {Time}");
            Log.Error($"Player.OnUpdate - {Time}");

            Vector3 velocity = Vector3.Zero;

            if (Input.IsKeyDown(KeyCode.W))
                velocity.Y = 1.0f;
            else if (Input.IsKeyDown(KeyCode.S))
                velocity.Y = -1.0f;

            if (Input.IsKeyDown(KeyCode.A))
                velocity.X = -1.0f;
            else if (Input.IsKeyDown(KeyCode.D))
                velocity.X = 1.0f;

            velocity *= Speed * ts;

            m_Rigidbody.ApplyLinearImpulse(velocity.XY, true);
        }
    }
}