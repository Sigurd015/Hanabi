using Hanabi;

namespace Sandbox
{
    public class Camera : Entity
    {
        public float DistanceFromPlayer = -5.0f;

        private Entity m_Player;

        void OnCreate()
        {
            m_Player = FindEntityByName("Player");
        }

        void OnUpdate(float ts)
        {
            if (m_Player != null)
                Transform.Translation = new Vector3(m_Player.Transform.Translation.XY, DistanceFromPlayer);
        }
    }
}