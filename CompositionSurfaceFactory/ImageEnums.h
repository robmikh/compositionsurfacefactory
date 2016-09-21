#pragma once

namespace Robmikh
{
    namespace CompositionSurfaceFactory
    {
        public enum class InterpolationMode
        {
            NearestNeighbor = 0,
            Linear = 1,
            Cubic = 2,
            MultiSampleLinear = 3,
            Anisotropic = 4,
            HighQualityCubic = 5
        };

        private class InterpolationModeHelper
        {
        public:
            static CanvasImageInterpolation GetCanvasImageInterpolation(CompositionSurfaceFactory::InterpolationMode interpolation)
            {
                switch (interpolation)
                {
                case CompositionSurfaceFactory::InterpolationMode::NearestNeighbor:
                    return CanvasImageInterpolation::NearestNeighbor;
                case CompositionSurfaceFactory::InterpolationMode::Linear:
                    return CanvasImageInterpolation::Linear;
                case CompositionSurfaceFactory::InterpolationMode::Cubic:
                    return CanvasImageInterpolation::Cubic;
                case CompositionSurfaceFactory::InterpolationMode::MultiSampleLinear:
                    return CanvasImageInterpolation::MultiSampleLinear;
                case CompositionSurfaceFactory::InterpolationMode::Anisotropic:
                    return CanvasImageInterpolation::Anisotropic;
                case CompositionSurfaceFactory::InterpolationMode::HighQualityCubic:
                    return CanvasImageInterpolation::HighQualityCubic;
                default:
                    return CanvasImageInterpolation::Linear;
                }
            }
            static CompositionSurfaceFactory::InterpolationMode GetInterpolationMode(CanvasImageInterpolation interpolation)
            {
                switch (interpolation)
                {
                case CanvasImageInterpolation::NearestNeighbor:
                    return CompositionSurfaceFactory::InterpolationMode::NearestNeighbor;
                case CanvasImageInterpolation::Linear:
                    return CompositionSurfaceFactory::InterpolationMode::Linear;
                case CanvasImageInterpolation::Cubic:
                    return CompositionSurfaceFactory::InterpolationMode::Cubic;
                case CanvasImageInterpolation::MultiSampleLinear:
                    return CompositionSurfaceFactory::InterpolationMode::MultiSampleLinear;
                case CanvasImageInterpolation::Anisotropic:
                    return CompositionSurfaceFactory::InterpolationMode::Anisotropic;
                case CanvasImageInterpolation::HighQualityCubic:
                    return CompositionSurfaceFactory::InterpolationMode::HighQualityCubic;
                default:
                    return CompositionSurfaceFactory::InterpolationMode::Linear;
                }
            }
        };
    }
}