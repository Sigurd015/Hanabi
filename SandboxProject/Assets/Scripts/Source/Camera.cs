using Hanabi;
using System;

namespace Sandbox
{
    public class Camera : Entity
    {
        public float DistanceFromPlayer = -5.0f;
        public float Speed = 5.0f;
        public float RotationSpeed = 5.0f;

        private Entity m_Player;

        void OnCreate()
        {
            //m_Player = FindEntityByName("Player");
        }

        void OnUpdate(float ts)
        {
            //if (m_Player != null)
            //Transform.Translation = new Vector3(m_Player.Transform.Translation.XY, DistanceFromPlayer);

            Vector3 pos = Vector3.Zero;
            Vector3 rot = Vector3.Zero;

            float rotSpeed = (float)(RotationSpeed * (Math.PI / 180.0));
            if (Input.IsKeyDown(KeyCode.W))
            {
                pos.Z += Speed;
            }
            else if (Input.IsKeyDown(KeyCode.S))
            {
                pos.Z -= Speed;
            }

            if (Input.IsKeyDown(KeyCode.A))
            {
                pos.X -= Speed;
            }
            else if (Input.IsKeyDown(KeyCode.D))
            {
                pos.X += Speed;
            }

            if (Input.IsKeyDown(KeyCode.Q))
            {
                rot.Y -= rotSpeed;
            }
            else if (Input.IsKeyDown(KeyCode.E))
            {
                rot.Y += rotSpeed;
            }

            if (Input.IsKeyDown(KeyCode.T))
            {
                rot.X -= rotSpeed;
            }
            else if (Input.IsKeyDown(KeyCode.G))
            {
                rot.X += rotSpeed;
            }

            Transform.Translation += pos * ts;
            Transform.Rotation += rot * ts;
        }
    }
}