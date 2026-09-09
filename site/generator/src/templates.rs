use std::fs;
use std::io::{Write, stdout};
use std::path::{Path, PathBuf};
use std::sync::{Arc, RwLock};

use handlebars::Handlebars;
use serde_json::Value;

use crate::config;
use crate::config::BuildConfig;
use crate::content;
use crate::diff;
use crate::markdown;
use crate::templates;

static COLLAPSIBLE_CARD_START_TEMPLATE: &str =
"<p class=\"d-inline-flex gap-1\">
  <button class=\"btn btn-primary\" type=\"button\" data-bs-toggle=\"collapse\" data-bs-target=\"#var\" aria-expanded=\"false\" aria-controls=\"var\">
    Expand: CARD_START_TEMPLATE_TEXT
  </button>
</p>
<div class=\"collapse\" id=\"var\">
<div class=\"card card-body\">";

static COLLAPSIBLE_CARD_END_TEMPLATE: &str = "</div>
</div>
<br />";

static CARD_START_TEMPLATE: &str = "<div class=\"card card-body\">
";

static CARD_END_TEMPLATE: &str = "</div>
<br />";

// implement by a structure impls HelperDef
#[derive(Clone)]
pub struct CollapsibleCardStartHelper {
    pub i: Arc<RwLock<u64>>,
}

impl handlebars::HelperDef for CollapsibleCardStartHelper {
    fn call<'reg: 'rc, 'rc>(
        &self,
        h: &handlebars::Helper,
        _: &Handlebars,
        _: &handlebars::Context,
        _rc: &mut handlebars::RenderContext,
        out: &mut dyn handlebars::Output,
    ) -> handlebars::HelperResult {
        let mut i = self.i.write().unwrap();
        *i += 1;

        let i = i.to_string();

        let card_text = h.param(0).unwrap().value().as_str().unwrap();
        let card_start_html = COLLAPSIBLE_CARD_START_TEMPLATE
            .replace("var", &i.to_string())
            .replace("CARD_START_TEMPLATE_TEXT", card_text);

        out.write(&card_start_html).unwrap();
        Ok(())
    }
}

pub fn collapsible_card_end_helper(
    _h: &handlebars::Helper,
    _: &Handlebars,
    _: &handlebars::Context,
    _rc: &mut handlebars::RenderContext,
    out: &mut dyn handlebars::Output,
) -> handlebars::HelperResult {
    out.write(COLLAPSIBLE_CARD_END_TEMPLATE).unwrap();
    Ok(())
}

pub fn card_start_helper(
    _h: &handlebars::Helper,
    _: &Handlebars,
    _: &handlebars::Context,
    _rc: &mut handlebars::RenderContext,
    out: &mut dyn handlebars::Output,
) -> handlebars::HelperResult {
    out.write(CARD_START_TEMPLATE).unwrap();
    Ok(())
}

pub fn card_end_helper(
    _h: &handlebars::Helper,
    _: &Handlebars,
    _: &handlebars::Context,
    _rc: &mut handlebars::RenderContext,
    out: &mut dyn handlebars::Output,
) -> handlebars::HelperResult {
    out.write(CARD_END_TEMPLATE).unwrap();
    Ok(())
}

pub fn image_helper(
    h: &handlebars::Helper,
    _: &Handlebars,
    _: &handlebars::Context,
    _rc: &mut handlebars::RenderContext,
    out: &mut dyn handlebars::Output,
) -> handlebars::HelperResult {
    let description = h.param(0).unwrap();
    let url = h.param(1).unwrap();

    let img_html = format!(
        "<img src={} class=\"img-fluid\" alt=\"{}\">",
        description.value().as_str().unwrap(),
        url.value().as_str().unwrap()
    );

    out.write(&img_html).unwrap();
    Ok(())
}

pub fn get_specific_content_context(
    handlebars: &Handlebars<'_>,
    template_context: &serde_json::Map<String, Value>,
    inserts: &Vec<(String, String)>,
    content: &content::Content,
) -> anyhow::Result<serde_json::Map<String, Value>> {
    let mut map: serde_json::Map<String, Value> = template_context.clone();
    let mut current_content = content::get_content_info(content);

    let content_title = content.front_matter["title"].as_str().unwrap().to_string();

    let (current_content, inserts) = {
        let mut temp_map = {
            let mut temp_map = map.clone();
            let temp_current_context = current_content.clone();
            temp_map.insert("current_content".to_string(), temp_current_context.into());
            temp_map
        };

        let rendered_content_html = {
            let (rendered_html, toc_items) =
                markdown::parse_markdown_to_html(&content_title, &content.markdown)?;

            let rendered_html = format!("{}\n{}", config::NO_ESCAPE, rendered_html);

            temp_map.insert("table_of_contents".to_string(), toc_items.unwrap());
            rendered_html
        };

        let inserts_value = {
            let context_for_inserts: Value = temp_map.clone().into();

            let mut insert_objects: serde_json::Map<String, Value> = serde_json::Map::new();

            for (name, html) in inserts {
                insert_objects.insert(
                    name.clone(),
                    handlebars
                        .render_template(html, &context_for_inserts)
                        .unwrap()
                        .into(),
                );
            }
            insert_objects
        };

        temp_map.insert("inserts".to_string(), inserts_value.clone().into());

        // We now have everything we might need to re-render the markdown into HTML
        let context_for_content_render: Value = temp_map.clone().into();

        let html = handlebars
            .render_template(&rendered_content_html, &context_for_content_render)
            .unwrap();

        current_content.insert("rendered_html".to_string(), html.into());
        (current_content.into(), inserts_value.into())
    };

    map.insert("inserts".to_string(), inserts);
    map.insert("current_content".to_string(), current_content);

    Ok(map)
}

pub fn handlebars_escape(data: &str) -> String {
    if data.contains(config::NO_ESCAPE) {
        return data.to_owned();
    }

    handlebars::html_escape(data)
}

pub fn process_content(config: &BuildConfig) -> anyhow::Result<Vec<(PathBuf, String)>> {
    let output_dir = &config.output_dir;
    let code_dir = &config.code_source_dir;
    let template_dir = &config.template_dir;
    let rendered_html: Vec<(PathBuf, String)> = Vec::new();

    let contents = content::get_content(config)?;
    let template_context = content::get_template_context(&contents);

    let mut handlebars: Handlebars<'_> = Handlebars::new();
    handlebars.register_escape_fn(handlebars_escape);
    handlebars.set_prevent_indent(true);

    handlebars.register_helper("img", Box::new(templates::image_helper));
    handlebars.register_helper(
        "collapsible-card",
        Box::new(templates::CollapsibleCardStartHelper {
            i: Arc::new(RwLock::new(0)),
        }),
    );
    handlebars.register_helper(
        "collapsible-card-end",
        Box::new(templates::collapsible_card_end_helper),
    );
    handlebars.register_helper("card", Box::new(templates::card_start_helper));
    handlebars.register_helper("card-end", Box::new(templates::card_end_helper));

    let inserts = content::get_inserts(config).unwrap();

    for i in 0..contents.len() {
        let content = &contents[i];
        let template = content.front_matter["template"].as_str().unwrap();

        let previous_content = if (i > 0) && (template == "lesson_template.html") {
            Some(&contents[i - 1])
        } else {
            None
        };

        println!("Processing {} content", content.file_name);
        stdout().flush().unwrap();

        let template_path = template_dir.join(template);
        let template_html = fs::read_to_string(&template_path).unwrap();
        let mut current_content_context =
            get_specific_content_context(&handlebars, &template_context, &inserts, &content)?;

        // TODO: Probably should figure out a way to not hardcode this.
        if let Some(previous_content) = previous_content {
            let previous_template = previous_content.front_matter["template"].as_str().unwrap();
            if previous_template == "lesson_template.html" {
                let content_name = Path::new(&content.file_name).file_stem().unwrap();
                let previous_content_name =
                    Path::new(&previous_content.file_name).file_stem().unwrap();

                let html = diff::diff_and_highlight(
                    &code_dir
                        .join(previous_content_name)
                        .join(format!("{}.c", previous_content_name.display())),
                    &code_dir
                        .join(content_name)
                        .join(format!("{}.c", content_name.display())),
                )?;

                current_content_context.insert("lesson_diff".to_string(), html.into());
            }
        }

        let current_content_context: Value = current_content_context.into();

        let final_html = handlebars
            .render_template(&template_html, &current_content_context)
            .unwrap();

        let final_file_path = output_dir.join(&content.file_path).with_extension("html");
        fs::create_dir_all(final_file_path.parent().unwrap()).unwrap();

        std::fs::write(final_file_path, final_html).unwrap();
    }

    Ok(rendered_html)
}
