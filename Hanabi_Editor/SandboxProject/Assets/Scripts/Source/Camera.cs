using Hanabi;

namespace Sandbox
{
    public class Camera : Entity
    {
        public Entity OtherEntity;

        public float DistanceFromPlayer = 5.0f;

        private Entity m_Player;

        void OnCreate()
        {
            m_Player = FindEntityByName("Player");
        }

        void OnUpdate(float ts)
        {
            if (m_Player != null)
                Translation = new Vector3(m_Player.Translation.XY, DistanceFromPlayer);
        }
    }
}