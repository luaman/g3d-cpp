/** 
  @file SuperShader.h

  A one-size-fits-all shader that combines most common illumination effects
  efficiently.

  @author Morgan McGuire, morgan@cs.brown.edu
 */

#ifndef SUPERSHADER_H
#define SUPERSHADER_H

#include <G3DAll.h>

typedef ReferenceCountedPointer<class SuperShader> SuperShaderRef;

/**
 Do not set the shader->args on a SuperShader; they are ignored.
 Instead set the material properties and lighting environment.
 */
// TODO: subclass _Shader
class SuperShader : public ReferenceCountedObject {
public:
    /** Material property coefficients are specified as 
        a constant color times a texture map.  If the color
        is white the texture map entirely controls the result.
        If the color is black the term is disabled.  On graphics
        cards with few texture units the map will be ignored.*/
    class Component {
    public:
        /** Color that is constant over the entire surface. */
        Color3              constant;

        /** Color that varies over position.  NULL means white.*/
        TextureRef          map;

        inline Component() : constant(Color3::black()), map(NULL) {}
        inline Component(const Color3& c) : constant(c), map(NULL) {}
        inline Component(double c) : constant(c,c,c), map(NULL) {}

        /** True if this material is definitely (0,0,0) everywhere */
        inline bool isBlack() const {
            return (constant.r == 0) && (constant.g == 0) && (constant.b == 0);
        }

        /** True if this material is definitely (1,1,1) everywhere */
        inline bool isWhite() const {
            return (constant.r == 1) && (constant.g == 1) && (constant.b == 1) && map.isNull();
        }

        /** Returns true if both components will produce similar non-zero terms in a
            lighting equation */
        inline bool similarTo(const Component& other) const{
            return 
                (isBlack() && other.isBlack()) ||
                (map.isNull() == other.map.isNull());
        }
    };

    /** Beta API; subject to change in future releases.
        Illumination Equation:

        dst1 = underlying value in frame buffer
        evt = environment map
        ambUp, ambDn = ambient map up and down values (ideally, environment map convolved with a hemisphere)

        dst2 = dst1 * transmission + 
               evt[n] * reflection +
               lerp(ambDn, ambUp, n.y/2 + 0.5) * diffuse +
               emissive +
               SUM OVER LIGHTS {
                 light * (diffuse * NdotL +
                          specular * NdotH^specularExponent)}

        When choosing material properties, the transmission, diffuse, and specular terms
        should sum to less than 1.  The reflection and specular terms are technically the
        same value and should be equal; the difference is that specular only applies to
        lights and reflection to the environment (less lights), a concession to artistic
        control.

        Note that most translucent materials should be two-sided and have comparatively
        low diffuse terms.  They should also be applied to convex objects (subdivide 
        non-convex objects) to prevent rendering surfaces out of order.

        */
	class Material {
	public:
        /** Diffuse reflection of lights */
        Component               diffuse;

        /** Glow */
        Component               emit;

        /** Specular (glossy) reflection of lights. */
        Component               specular;

        /** Sharpness of light reflections.*/
        Component               specularExponent;


        Component               transmit;

        /** Perfect specular (mirror) reflection of the environment */
        Component               reflect;

        /** RGB*2-1 = tangent space normal, A = tangent space bump height.
          If NULL bump mapping is disabled. */
        TextureRef              normalMap;
       
        /** Multiply normal map alpha values (originally on the range 0-1)
            by this constant to obtain the real-world bump height. Should
            already be factored in to the normal map normals.*/
        float                   bumpMapScale;
        
        Material() : diffuse(1), emit(0), 
            specular(0.25), specularExponent(60), 
            transmit(0), reflect(0) {
        }

        /** Configures the material arguments on a SuperShader for
            the opaque pass. */
        void configureShaderArgs(VertexAndPixelShader::ArgList& args) const;

        /** Returns true if this material uses similar terms as other
            (used by SuperShader), although the actual textures may differ. */
        bool similarTo(const Material& other) const;
	};

    class LightingEnvironment : public ReferenceCountedObject {
    private:
    
        LightingEnvironment() {}

    public:

        /** Light reflected from the sky (usually slightly blue) */
        Color3              ambientTop;

        /** Light reflected from the ground.  A simpler code path is taken
            if identical to ambientTop. */
        Color3              ambientBottom;

        Component           environment;

        Array<GLight>       lightArray;

        /** Creates a (dark) environment. */
        static ReferenceCountedPointer<LightingEnvironment> create() {
            return new LightingEnvironment();
        }

        /** 
         Configures from a sky and time.  The fields may also be set explicitly.
         The ambient values are set from the lighting parameters.  
         If the sky is NULL the sky settings
         are not changed.
         */
        void set(GameTime time, SkyRef sky = NULL);

        /** Configures the lighting arguments on a SuperShader.*/
        void configureShaderArgs(VertexAndPixelShader::ArgList& args) const;
    };

    typedef ReferenceCountedPointer<LightingEnvironment> LightingEnvironmentRef;

private:
    class Cache {
    private:

        Array<Material>     materialArray;
        Array<ShaderRef>    shaderArray;

    public:

        // TODO: mechansim for purging old shaders

        /** Adds a shader to the list of cached ones.  Only call when 
            getSimilar returned NULL.*/
        void add(const Material& mat, ShaderRef shader);

        /** Returns the shader for a similar material or 
            NULL if one does not exist. */
        ShaderRef getSimilar(const Material& mat) const;
    };

    static Cache cache;

    /** Classification of a graphics card. 
        FIXED_FUNCTION  Use OpenGL fixed function lighting only.
        PS14            
        PS20            Use pixel shader 2.0 (full feature)
     */
    enum GraphicsProfile {
        UNKNOWN = 0,
        FIXED_FUNCTION,
        PS20};

    /** Measures the capabilities of this machine */
    inline static GraphicsProfile profile() {
        static GraphicsProfile p = UNKNOWN;

        if (p == UNKNOWN) {
            if (GLCaps::supports_GL_ARB_shader_objects()) {
                p = PS20;
            } else {
                p = FIXED_FUNCTION;
            }
p = FIXED_FUNCTION; // TODO: remove
        }

        return p;
    }

    /** Returns the SuperShader for this material, with arguments set. */
    static ShaderRef getShader(const Material& material);

    /** Configuration for a non-programmable card.
        No reflection map, single ambient color. */
    void configureFixedFunction(RenderDevice* rd);


    LightingEnvironmentRef  lighting;

    Material                material;

    /** Underlying shader.  May be shared between multiple SuperShaders. */
    ShaderRef               shader;

    explicit SuperShader(const Material& material);

public:

    static SuperShaderRef create(const Material& material);

    virtual bool ok() const;
    virtual void beforePrimitive(RenderDevice* renderDevice);
    virtual void afterPrimitive(RenderDevice* renderDevice);
    virtual const std::string& messages () const;

    /** Sets all lighting parameters from this lighting environment. The environment
        will be pointed at, so future changes are automatically reflected without
        another call. */
    void setLighting(const LightingEnvironmentRef& lightingEnvironment);

}; // SuperShader

#endif
